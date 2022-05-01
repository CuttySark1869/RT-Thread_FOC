#include "mc_transform.h"

#include "mc_encoder.h"

#include <stdint.h>
#include <math.h>

#define TABLE_SIZE              (256u)
#define TOTAL_SINE_TABLE_ANGLE  (2*M_PI)
#define ANGLE_STEP              (TOTAL_SINE_TABLE_ANGLE / (float)TABLE_SIZE)
#define ONE_BY_ANGLE_STEP       (1 / ANGLE_STEP)

/******************************************************************************/
/*                   SIN Table  256  -  0.0244rad resolution                            */
/******************************************************************************/
float sine_table[TABLE_SIZE] =
{
    0.000000,  0.024541,  0.049068,  0.073565,  0.098017,  0.122411,  0.146730,  0.170962,
    0.195090,  0.219101,  0.242980,  0.266713,  0.290285,  0.313682,  0.336890,  0.359895,
    0.382683,  0.405241,  0.427555,  0.449611,  0.471397,  0.492898,  0.514103,  0.534998,
    0.555570,  0.575808,  0.595699,  0.615232,  0.634393,  0.653173,  0.671559,  0.689541,
    0.707107,  0.724247,  0.740951,  0.757209,  0.773010,  0.788346,  0.803208,  0.817585,
    0.831470,  0.844854,  0.857729,  0.870087,  0.881921,  0.893224,  0.903989,  0.914210,
    0.923880,  0.932993,  0.941544,  0.949528,  0.956940,  0.963776,  0.970031,  0.975702,
    0.980785,  0.985278,  0.989177,  0.992480,  0.995185,  0.997290,  0.998795,  0.999699,
    1.000000,  0.999699,  0.998795,  0.997290,  0.995185,  0.992480,  0.989177,  0.985278,
    0.980785,  0.975702,  0.970031,  0.963776,  0.956940,  0.949528,  0.941544,  0.932993,
    0.923880,  0.914210,  0.903989,  0.893224,  0.881921,  0.870087,  0.857729,  0.844854,
    0.831470,  0.817585,  0.803208,  0.788346,  0.773010,  0.757209,  0.740951,  0.724247,
    0.707107,  0.689541,  0.671559,  0.653173,  0.634393,  0.615232,  0.595699,  0.575808,
    0.555570,  0.534998,  0.514103,  0.492898,  0.471397,  0.449611,  0.427555,  0.405241,
    0.382683,  0.359895,  0.336890,  0.313682,  0.290285,  0.266713,  0.242980,  0.219101,
    0.195090,  0.170962,  0.146730,  0.122411,  0.098017,  0.073565,  0.049068,  0.024541,
    0.000000, -0.024541, -0.049068, -0.073565, -0.098017, -0.122411, -0.146730, -0.170962,
   -0.195090, -0.219101, -0.242980, -0.266713, -0.290285, -0.313682, -0.336890, -0.359895,
   -0.382683, -0.405241, -0.427555, -0.449611, -0.471397, -0.492898, -0.514103, -0.534998,
   -0.555570, -0.575808, -0.595699, -0.615232, -0.634393, -0.653173, -0.671559, -0.689541,
   -0.707107, -0.724247, -0.740951, -0.757209, -0.773010, -0.788346, -0.803208, -0.817585,
   -0.831470, -0.844854, -0.857729, -0.870087, -0.881921, -0.893224, -0.903989, -0.914210,
   -0.923880, -0.932993, -0.941544, -0.949528, -0.956940, -0.963776, -0.970031, -0.975702,
   -0.980785, -0.985278, -0.989177, -0.992480, -0.995185, -0.997290, -0.998795, -0.999699,
   -1.000000, -0.999699, -0.998795, -0.997290, -0.995185, -0.992480, -0.989177, -0.985278,
   -0.980785, -0.975702, -0.970031, -0.963776, -0.956940, -0.949528, -0.941544, -0.932993,
   -0.923880, -0.914210, -0.903989, -0.893224, -0.881921, -0.870087, -0.857729, -0.844854,
   -0.831470, -0.817585, -0.803208, -0.788346, -0.773010, -0.757209, -0.740951, -0.724247,
   -0.707107, -0.689541, -0.671559, -0.653173, -0.634393, -0.615232, -0.595699, -0.575808,
   -0.555570, -0.534998, -0.514103, -0.492898, -0.471397, -0.449611, -0.427555, -0.405241,
   -0.382683, -0.359895, -0.336890, -0.313682, -0.290285, -0.266713, -0.242980, -0.219101,
   -0.195090, -0.170962, -0.146730, -0.122411, -0.098017, -0.073565, -0.049068, -0.024541
};

/******************************************************************************/
/*                   COS Table  -  0.0244rad resolution                                     */
/******************************************************************************/
float cosine_table[TABLE_SIZE] =
{
    1.000000,  0.999699,  0.998795,  0.997290,  0.995185,  0.992480,  0.989177,  0.985278,
    0.980785,  0.975702,  0.970031,  0.963776,  0.956940,  0.949528,  0.941544,  0.932993,
    0.923880,  0.914210,  0.903989,  0.893224,  0.881921,  0.870087,  0.857729,  0.844854,
    0.831470,  0.817585,  0.803208,  0.788346,  0.773010,  0.757209,  0.740951,  0.724247,
    0.707107,  0.689541,  0.671559,  0.653173,  0.634393,  0.615232,  0.595699,  0.575808,
    0.555570,  0.534998,  0.514103,  0.492898,  0.471397,  0.449611,  0.427555,  0.405241,
    0.382683,  0.359895,  0.336890,  0.313682,  0.290285,  0.266713,  0.242980,  0.219101,
    0.195090,  0.170962,  0.146730,  0.122411,  0.098017,  0.073565,  0.049068,  0.024541,
    0.000000, -0.024541, -0.049068, -0.073565, -0.098017, -0.122411, -0.146730, -0.170962,
   -0.195090, -0.219101, -0.242980, -0.266713, -0.290285, -0.313682, -0.336890, -0.359895,
   -0.382683, -0.405241, -0.427555, -0.449611, -0.471397, -0.492898, -0.514103, -0.534998,
   -0.555570, -0.575808, -0.595699, -0.615232, -0.634393, -0.653173, -0.671559, -0.689541,
   -0.707107, -0.724247, -0.740951, -0.757209, -0.773010, -0.788346, -0.803208, -0.817585,
   -0.831470, -0.844854, -0.857729, -0.870087, -0.881921, -0.893224, -0.903989, -0.914210,
   -0.923880, -0.932993, -0.941544, -0.949528, -0.956940, -0.963776, -0.970031, -0.975702,
   -0.980785, -0.985278, -0.989177, -0.992480, -0.995185, -0.997290, -0.998795, -0.999699,
   -1.000000, -0.999699, -0.998795, -0.997290, -0.995185, -0.992480, -0.989177, -0.985278,
   -0.980785, -0.975702, -0.970031, -0.963776, -0.956940, -0.949528, -0.941544, -0.932993,
   -0.923880, -0.914210, -0.903989, -0.893224, -0.881921, -0.870087, -0.857729, -0.844854,
   -0.831470, -0.817585, -0.803208, -0.788346, -0.773010, -0.757209, -0.740951, -0.724247,
   -0.707107, -0.689541, -0.671559, -0.653173, -0.634393, -0.615232, -0.595699, -0.575808,
   -0.555570, -0.534998, -0.514103, -0.492898, -0.471397, -0.449611, -0.427555, -0.405241,
   -0.382683, -0.359895, -0.336890, -0.313682, -0.290285, -0.266713, -0.242980, -0.219101,
   -0.195090, -0.170962, -0.146730, -0.122411, -0.098017, -0.073565, -0.049068, -0.024541,
    0.000000,  0.024541,  0.049068,  0.073565,  0.098017,  0.122411,  0.146730,  0.170962,
    0.195090,  0.219101,  0.242980,  0.266713,  0.290285,  0.313682,  0.336890,  0.359895,
    0.382683,  0.405241,  0.427555,  0.449611,  0.471397,  0.492898,  0.514103,  0.534998,
    0.555570,  0.575808,  0.595699,  0.615232,  0.634393,  0.653173,  0.671559,  0.689541,
    0.707107,  0.724247,  0.740951,  0.757209,  0.773010,  0.788346,  0.803208,  0.817585,
    0.831470,  0.844854,  0.857729,  0.870087,  0.881921,  0.893224,  0.903989,  0.914210,
    0.923880,  0.932993,  0.941544,  0.949528,  0.956940,  0.963776,  0.970031,  0.975702,
    0.980785,  0.985278,  0.989177,  0.992480,  0.995185,  0.997290,  0.998795,  0.999699
};



void mc_calc_sin_cos(float const rotor_angle, float* sin_angle, float* cos_angle)
{
    float angle = rotor_angle;
    /*
       Since we are using "float", it is not possible to get an index of array
       directly. Almost every time, we will need to do interpolation, as per
       following equation: -
       y = y0 + (y1 - y0)*((x - x0)/(x1 - x0)) */

    uint32_t y0_index;
    uint32_t y0_index_next;
    float x0, y0, y1, temp;

    // Software check to ensure  0 <= Angle < 2*PI
    if( angle <  0 )
    {
        angle = angle + TOTAL_SINE_TABLE_ANGLE;
    }

    if( angle >= TOTAL_SINE_TABLE_ANGLE  )
    {
        angle = angle - TOTAL_SINE_TABLE_ANGLE;
    }


    y0_index = (uint32_t)(angle / ANGLE_STEP);
    y0_index_next = y0_index + 1;

    if(y0_index_next >= TABLE_SIZE )
    {
        y0_index_next = 0;
    }

    x0 = (y0_index * ANGLE_STEP);

    /* Since below calculation is same for sin & cosine, we can do it once and reuse. */
    temp = ((angle - x0) * ONE_BY_ANGLE_STEP);

    /*==============  Find Sine now  =============================================*/
    y0 = sine_table[y0_index];
    y1 = sine_table[y0_index_next];
    *sin_angle = y0 + ((y1 - y0) * temp);

   /*==============  Find Cosine now  =============================================*/
    y0 = cosine_table[y0_index];
    y1 = cosine_table[y0_index_next];
    *cos_angle = y0 + ((y1 - y0) * temp);

}

void mc_wrap_angle(float * const angle)
{
    if(*angle >= TOTAL_SINE_TABLE_ANGLE)
    {
        *angle -= TOTAL_SINE_TABLE_ANGLE;
    }
    else if( 0.0f > *angle )
    {
        *angle += TOTAL_SINE_TABLE_ANGLE;
    }
    else
    {
       /* Do nothing */
    }
}



void mc_linear_ramp(float * const input, const float step_size, const float final_val)
{
    if ((*input + step_size ) < final_val )
    {
        *input += step_size;
    }
    else if( (*input - step_size ) > final_val )
    {
        *input -= step_size;
    }
    else
    {
      *input = final_val;
    }
}


void mc_impose_limits(float * const input, const float limit_lower, const float limit_upper)
{
    if( *input > limit_upper )
    {
        *input  = limit_upper;
    }
    else if( *input < limit_lower )
    {
        *input  = limit_lower;
    }
}



void mc_clarke_park_transform(mc_input_signals_t *input, mc_tansform_t *output)
{
    output->clarke.alpha = input->ia;
    output->clarke.beta = (input->ia * ONE_BY_SQRT3) + (input->ib * TWO_BY_SQRT3);

    mc_calc_sin_cos(input->e_angle, &output->sin_angle, &output->cos_angle);

    output->park.d_axis =  output->clarke.alpha * output->cos_angle
                        + output->clarke.beta * output->sin_angle;

    output->park.d_axis = -output->clarke.alpha * output->sin_angle
                        + output->clarke.beta * output->cos_angle;
}

/******************************************************************************/
/* Function name: MCLIB_InvParkTransform                                      */
/* Function parameters: input - park transform inputs                         */
/*                      output - clarke transform output                      */
/*                      position - rotor angle                                */
/* Function return: None                                                      */
/* Description: Inverse Park Transformation.                                  */
/******************************************************************************/
void mc_inverse_park_transform(mc_tansform_t *transform)
{
     transform->clarke.alpha =  transform->park.d_axis * transform->cos_angle - transform->park.q_axis * transform->sin_angle;
     transform->clarke.beta  =  transform->park.d_axis * transform->sin_angle + transform->park.q_axis * transform->cos_angle;
}

