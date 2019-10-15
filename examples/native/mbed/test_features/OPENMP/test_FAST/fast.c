#include "omp.h"
#include "fast.h"

/* THRESHOLD used on circular_detection */
#define FAST_THRESHOLD 255

void make_offsets(int32_t offset[], uint32_t width)
{
    offset[0] = 0 + width * 3;
    offset[1] = 1 + width * 3;
    offset[2] = 2 + width * 2;
    offset[3] = 3 + width * 1;
    offset[4] = 3 + width * 0;
    offset[5] = 3 + width * -1;
    offset[6] = 2 + width * -2;
    offset[7] = 1 + width * -3;
    offset[8] = 0 + width * -3;
    offset[9] = -1 + width * -3;
    offset[10] = -2 + width * -2;
    offset[11] = -3 + width * -1;
    offset[12] = -3 + width * 0;
    offset[13] = -3 + width * 1;
    offset[14] = -2 + width * 2;
    offset[15] = -1 + width * 3;
    return;
}

/* Perform circular detection on input looking for corners */
static inline void circular_detection(int32_t x,
                                      int32_t y,
                                      uint32_t *corners_found,
                                      uint32_t *corners,
                                      IMG_DATATYPE *input,
                                      uint32_t width,
                                      int32_t *offsets,
                                      int32_t Y)
{
#ifdef UNOPTIMIZED
    int32_t iteration, found, n;
    uint32_t p0, p1;
    IMG_DATATYPE intensity;
    uint32_t loop;
    uint32_t repeat, min, max, threshold, score, score_max;
    uint32_t skipUpDetect, skipDownDetect;
    int32_t  xy = y*width+x;
    uint32_t corner_id;

    intensity = input[xy];

    p0 = intensity - FAST_THRESHOLD;
    p1 = intensity + FAST_THRESHOLD;

    // Check luminance overflow
    skipUpDetect = p1 > 254 ? 1 : 0;
    skipDownDetect = p0 < 1 ? 1 : 0;

    loop = 1;
    iteration = 0;

    /* detection loop */
    while((iteration<16) && (loop==1))
    {
        found = 1;
        n = 0;

        // if threashold is lower than 0 this is useless
        if(!skipDownDetect)
            while ((found == 1) && (n<9))
                found = (input[xy + (offsets[(iteration + n++)%16])] < p0) ? 1 : 0;
        else
            found = 0;

        if (found != 0)
        {
            #pragma omp critical
            {
                corner_id = *corners_found;
                *(corners_found) += 1;
            }
            corners[(3*corner_id    )] = x;
            corners[(3*corner_id + 1)] = y+Y;
            loop = 0;
        }
        else
        {
            found = 1;
            n = 0;

            //if threashold is bigger than 255 this is useless
            if(!skipUpDetect)
                while ((found == 1) && (n<9))
                    found = (input[xy + (offsets[(iteration + n++)%16])] > p1);
            else
                found = 0;

            if (found != 0)
            {
                #pragma omp critical
                {
                    corner_id = *corners_found;
                    *(corners_found) += 1;
                }
                corners[(3*corner_id    )] = x;
                corners[(3*corner_id + 1)] = y+Y;
                loop = 0;
            }
        }

        iteration++;
    }

    repeat = 1;
    score_max = 0;

    /* score detection - only if corner is detected */
    if (loop == 0)
    {
        loop = 1;
        iteration = 0;
        score_max = 0;
        score = 0;

        while ((iteration<16) && (loop==1))
        {
            repeat = 1;
            min = FAST_THRESHOLD; //min luminance value
            max = 255;  //max luminance value

            while(repeat==1)
            {
                threshold = (min+max) >> 1; // /2
                p0 = intensity - threshold;
                p1 = intensity + threshold;
                skipUpDetect = p1 > 254 ? 1 : 0;
                skipDownDetect = p0 < 1 ? 1 : 0;
                n = 0;
                found = 1;

                // if threashold is lower than 0 this is useless
                if(!skipDownDetect)
                    while ((found == 1) && (n<9))
                        found = (input[xy + (offsets[(iteration + n++)%16])] < p0) ? 1 : 0;
                else
                    found = 0;

                if (found == 1)
                    min = threshold;
                else
                {
                    n = 0;
                    found = 1;

                    //if threashold is bigger than 255 this is useless
                    if(!skipUpDetect)
                        while ((found == 1) && (n<9))
                            found = (input[xy + (offsets[(iteration + n++)%16])] > p1) ? 1 : 0;
                    else
                        found = 0;

                    if (found == 1)
                        min = threshold;
                    else
                        max = threshold;
                }

                if ((min == max-1) || (min == max))
                {
                    repeat = 0;
                    score = min;
                }
            }
            score_max = (score > score_max) ? score : score_max;
            iteration++;
        }
        corners[(3*corner_id+2)] = score_max;
    }
#else
        uint32_t corner_id;
        IMG_DATATYPE *p = input + y*width + x;

        uint32_t cb = *p + FAST_THRESHOLD;
        uint32_t c_b= *p - FAST_THRESHOLD;

        if(p[offsets[0]] > cb)
         if(p[offsets[1]] > cb)
          if(p[offsets[2]] > cb)
           if(p[offsets[3]] > cb)
            if(p[offsets[4]] > cb)
             if(p[offsets[5]] > cb)
              if(p[offsets[6]] > cb)
               if(p[offsets[7]] > cb)
                if(p[offsets[8]] > cb)
                 {}
                else
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  return;
               else if(p[offsets[7]] < c_b)
                if(p[offsets[14]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  return;
                else if(p[offsets[14]] < c_b)
                 if(p[offsets[8]] < c_b)
                  if(p[offsets[9]] < c_b)
                   if(p[offsets[10]] < c_b)
                    if(p[offsets[11]] < c_b)
                     if(p[offsets[12]] < c_b)
                      if(p[offsets[13]] < c_b)
                       if(p[offsets[15]] < c_b)
                        {}
                       else
                        return;
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                if(p[offsets[14]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  return;
                else
                 return;
              else if(p[offsets[6]] < c_b)
               if(p[offsets[15]] > cb)
                if(p[offsets[13]] > cb)
                 if(p[offsets[14]] > cb)
                  {}
                 else
                  return;
                else if(p[offsets[13]] < c_b)
                 if(p[offsets[7]] < c_b)
                  if(p[offsets[8]] < c_b)
                   if(p[offsets[9]] < c_b)
                    if(p[offsets[10]] < c_b)
                     if(p[offsets[11]] < c_b)
                      if(p[offsets[12]] < c_b)
                       if(p[offsets[14]] < c_b)
                        {}
                       else
                        return;
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                if(p[offsets[7]] < c_b)
                 if(p[offsets[8]] < c_b)
                  if(p[offsets[9]] < c_b)
                   if(p[offsets[10]] < c_b)
                    if(p[offsets[11]] < c_b)
                     if(p[offsets[12]] < c_b)
                      if(p[offsets[13]] < c_b)
                       if(p[offsets[14]] < c_b)
                        {}
                       else
                        return;
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[13]] > cb)
                if(p[offsets[14]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  return;
                else
                 return;
               else if(p[offsets[13]] < c_b)
                if(p[offsets[7]] < c_b)
                 if(p[offsets[8]] < c_b)
                  if(p[offsets[9]] < c_b)
                   if(p[offsets[10]] < c_b)
                    if(p[offsets[11]] < c_b)
                     if(p[offsets[12]] < c_b)
                      if(p[offsets[14]] < c_b)
                       if(p[offsets[15]] < c_b)
                        {}
                       else
                        return;
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else if(p[offsets[5]] < c_b)
              if(p[offsets[14]] > cb)
               if(p[offsets[12]] > cb)
                if(p[offsets[13]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     if(p[offsets[9]] > cb)
                      if(p[offsets[10]] > cb)
                       if(p[offsets[11]] > cb)
                        {}
                       else
                        return;
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 return;
               else if(p[offsets[12]] < c_b)
                if(p[offsets[6]] < c_b)
                 if(p[offsets[7]] < c_b)
                  if(p[offsets[8]] < c_b)
                   if(p[offsets[9]] < c_b)
                    if(p[offsets[10]] < c_b)
                     if(p[offsets[11]] < c_b)
                      if(p[offsets[13]] < c_b)
                       {}
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else if(p[offsets[14]] < c_b)
               if(p[offsets[7]] < c_b)
                if(p[offsets[8]] < c_b)
                 if(p[offsets[9]] < c_b)
                  if(p[offsets[10]] < c_b)
                   if(p[offsets[11]] < c_b)
                    if(p[offsets[12]] < c_b)
                     if(p[offsets[13]] < c_b)
                      if(p[offsets[6]] < c_b)
                       {}
                      else
                       if(p[offsets[15]] < c_b)
                        {}
                       else
                        return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               if(p[offsets[6]] < c_b)
                if(p[offsets[7]] < c_b)
                 if(p[offsets[8]] < c_b)
                  if(p[offsets[9]] < c_b)
                   if(p[offsets[10]] < c_b)
                    if(p[offsets[11]] < c_b)
                     if(p[offsets[12]] < c_b)
                      if(p[offsets[13]] < c_b)
                       {}
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              if(p[offsets[12]] > cb)
               if(p[offsets[13]] > cb)
                if(p[offsets[14]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     if(p[offsets[9]] > cb)
                      if(p[offsets[10]] > cb)
                       if(p[offsets[11]] > cb)
                        {}
                       else
                        return;
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 return;
               else
                return;
              else if(p[offsets[12]] < c_b)
               if(p[offsets[7]] < c_b)
                if(p[offsets[8]] < c_b)
                 if(p[offsets[9]] < c_b)
                  if(p[offsets[10]] < c_b)
                   if(p[offsets[11]] < c_b)
                    if(p[offsets[13]] < c_b)
                     if(p[offsets[14]] < c_b)
                      if(p[offsets[6]] < c_b)
                       {}
                      else
                       if(p[offsets[15]] < c_b)
                        {}
                       else
                        return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
            else if(p[offsets[4]] < c_b)
             if(p[offsets[13]] > cb)
              if(p[offsets[11]] > cb)
               if(p[offsets[12]] > cb)
                if(p[offsets[14]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     if(p[offsets[9]] > cb)
                      if(p[offsets[10]] > cb)
                       {}
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     if(p[offsets[9]] > cb)
                      if(p[offsets[10]] > cb)
                       {}
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                return;
              else if(p[offsets[11]] < c_b)
               if(p[offsets[5]] < c_b)
                if(p[offsets[6]] < c_b)
                 if(p[offsets[7]] < c_b)
                  if(p[offsets[8]] < c_b)
                   if(p[offsets[9]] < c_b)
                    if(p[offsets[10]] < c_b)
                     if(p[offsets[12]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
             else if(p[offsets[13]] < c_b)
              if(p[offsets[7]] < c_b)
               if(p[offsets[8]] < c_b)
                if(p[offsets[9]] < c_b)
                 if(p[offsets[10]] < c_b)
                  if(p[offsets[11]] < c_b)
                   if(p[offsets[12]] < c_b)
                    if(p[offsets[6]] < c_b)
                     if(p[offsets[5]] < c_b)
                      {}
                     else
                      if(p[offsets[14]] < c_b)
                       {}
                      else
                       return;
                    else
                     if(p[offsets[14]] < c_b)
                      if(p[offsets[15]] < c_b)
                       {}
                      else
                       return;
                     else
                      return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
             else
              if(p[offsets[5]] < c_b)
               if(p[offsets[6]] < c_b)
                if(p[offsets[7]] < c_b)
                 if(p[offsets[8]] < c_b)
                  if(p[offsets[9]] < c_b)
                   if(p[offsets[10]] < c_b)
                    if(p[offsets[11]] < c_b)
                     if(p[offsets[12]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
            else
             if(p[offsets[11]] > cb)
              if(p[offsets[12]] > cb)
               if(p[offsets[13]] > cb)
                if(p[offsets[14]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     if(p[offsets[9]] > cb)
                      if(p[offsets[10]] > cb)
                       {}
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     if(p[offsets[9]] > cb)
                      if(p[offsets[10]] > cb)
                       {}
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                return;
              else
               return;
             else if(p[offsets[11]] < c_b)
              if(p[offsets[7]] < c_b)
               if(p[offsets[8]] < c_b)
                if(p[offsets[9]] < c_b)
                 if(p[offsets[10]] < c_b)
                  if(p[offsets[12]] < c_b)
                   if(p[offsets[13]] < c_b)
                    if(p[offsets[6]] < c_b)
                     if(p[offsets[5]] < c_b)
                      {}
                     else
                      if(p[offsets[14]] < c_b)
                       {}
                      else
                       return;
                    else
                     if(p[offsets[14]] < c_b)
                      if(p[offsets[15]] < c_b)
                       {}
                      else
                       return;
                     else
                      return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
             else
              return;
           else if(p[offsets[3]] < c_b)
            if(p[offsets[10]] > cb)
             if(p[offsets[11]] > cb)
              if(p[offsets[12]] > cb)
               if(p[offsets[13]] > cb)
                if(p[offsets[14]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     if(p[offsets[9]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     if(p[offsets[9]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[4]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     if(p[offsets[9]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               return;
             else
              return;
            else if(p[offsets[10]] < c_b)
             if(p[offsets[7]] < c_b)
              if(p[offsets[8]] < c_b)
               if(p[offsets[9]] < c_b)
                if(p[offsets[11]] < c_b)
                 if(p[offsets[6]] < c_b)
                  if(p[offsets[5]] < c_b)
                   if(p[offsets[4]] < c_b)
                    {}
                   else
                    if(p[offsets[12]] < c_b)
                     if(p[offsets[13]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                  else
                   if(p[offsets[12]] < c_b)
                    if(p[offsets[13]] < c_b)
                     if(p[offsets[14]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                 else
                  if(p[offsets[12]] < c_b)
                   if(p[offsets[13]] < c_b)
                    if(p[offsets[14]] < c_b)
                     if(p[offsets[15]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 return;
               else
                return;
              else
               return;
             else
              return;
            else
             return;
           else
            if(p[offsets[10]] > cb)
             if(p[offsets[11]] > cb)
              if(p[offsets[12]] > cb)
               if(p[offsets[13]] > cb)
                if(p[offsets[14]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     if(p[offsets[9]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     if(p[offsets[9]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[4]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     if(p[offsets[9]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               return;
             else
              return;
            else if(p[offsets[10]] < c_b)
             if(p[offsets[7]] < c_b)
              if(p[offsets[8]] < c_b)
               if(p[offsets[9]] < c_b)
                if(p[offsets[11]] < c_b)
                 if(p[offsets[12]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[5]] < c_b)
                    if(p[offsets[4]] < c_b)
                     {}
                    else
                     if(p[offsets[13]] < c_b)
                      {}
                     else
                      return;
                   else
                    if(p[offsets[13]] < c_b)
                     if(p[offsets[14]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                  else
                   if(p[offsets[13]] < c_b)
                    if(p[offsets[14]] < c_b)
                     if(p[offsets[15]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
             else
              return;
            else
             return;
          else if(p[offsets[2]] < c_b)
           if(p[offsets[9]] > cb)
            if(p[offsets[10]] > cb)
             if(p[offsets[11]] > cb)
              if(p[offsets[12]] > cb)
               if(p[offsets[13]] > cb)
                if(p[offsets[14]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[4]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[3]] > cb)
                if(p[offsets[4]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              return;
            else
             return;
           else if(p[offsets[9]] < c_b)
            if(p[offsets[7]] < c_b)
             if(p[offsets[8]] < c_b)
              if(p[offsets[10]] < c_b)
               if(p[offsets[6]] < c_b)
                if(p[offsets[5]] < c_b)
                 if(p[offsets[4]] < c_b)
                  if(p[offsets[3]] < c_b)
                   {}
                  else
                   if(p[offsets[11]] < c_b)
                    if(p[offsets[12]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                 else
                  if(p[offsets[11]] < c_b)
                   if(p[offsets[12]] < c_b)
                    if(p[offsets[13]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[11]] < c_b)
                  if(p[offsets[12]] < c_b)
                   if(p[offsets[13]] < c_b)
                    if(p[offsets[14]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[11]] < c_b)
                 if(p[offsets[12]] < c_b)
                  if(p[offsets[13]] < c_b)
                   if(p[offsets[14]] < c_b)
                    if(p[offsets[15]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               return;
             else
              return;
            else
             return;
           else
            return;
          else
           if(p[offsets[9]] > cb)
            if(p[offsets[10]] > cb)
             if(p[offsets[11]] > cb)
              if(p[offsets[12]] > cb)
               if(p[offsets[13]] > cb)
                if(p[offsets[14]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[4]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[3]] > cb)
                if(p[offsets[4]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    if(p[offsets[8]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              return;
            else
             return;
           else if(p[offsets[9]] < c_b)
            if(p[offsets[7]] < c_b)
             if(p[offsets[8]] < c_b)
              if(p[offsets[10]] < c_b)
               if(p[offsets[11]] < c_b)
                if(p[offsets[6]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[4]] < c_b)
                   if(p[offsets[3]] < c_b)
                    {}
                   else
                    if(p[offsets[12]] < c_b)
                     {}
                    else
                     return;
                  else
                   if(p[offsets[12]] < c_b)
                    if(p[offsets[13]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                 else
                  if(p[offsets[12]] < c_b)
                   if(p[offsets[13]] < c_b)
                    if(p[offsets[14]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[12]] < c_b)
                  if(p[offsets[13]] < c_b)
                   if(p[offsets[14]] < c_b)
                    if(p[offsets[15]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                return;
              else
               return;
             else
              return;
            else
             return;
           else
            return;
         else if(p[offsets[1]] < c_b)
          if(p[offsets[8]] > cb)
           if(p[offsets[9]] > cb)
            if(p[offsets[10]] > cb)
             if(p[offsets[11]] > cb)
              if(p[offsets[12]] > cb)
               if(p[offsets[13]] > cb)
                if(p[offsets[14]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[4]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[3]] > cb)
                if(p[offsets[4]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              if(p[offsets[2]] > cb)
               if(p[offsets[3]] > cb)
                if(p[offsets[4]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
            else
             return;
           else
            return;
          else if(p[offsets[8]] < c_b)
           if(p[offsets[7]] < c_b)
            if(p[offsets[9]] < c_b)
             if(p[offsets[6]] < c_b)
              if(p[offsets[5]] < c_b)
               if(p[offsets[4]] < c_b)
                if(p[offsets[3]] < c_b)
                 if(p[offsets[2]] < c_b)
                  {}
                 else
                  if(p[offsets[10]] < c_b)
                   if(p[offsets[11]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[10]] < c_b)
                  if(p[offsets[11]] < c_b)
                   if(p[offsets[12]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[10]] < c_b)
                 if(p[offsets[11]] < c_b)
                  if(p[offsets[12]] < c_b)
                   if(p[offsets[13]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[10]] < c_b)
                if(p[offsets[11]] < c_b)
                 if(p[offsets[12]] < c_b)
                  if(p[offsets[13]] < c_b)
                   if(p[offsets[14]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              if(p[offsets[10]] < c_b)
               if(p[offsets[11]] < c_b)
                if(p[offsets[12]] < c_b)
                 if(p[offsets[13]] < c_b)
                  if(p[offsets[14]] < c_b)
                   if(p[offsets[15]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
            else
             return;
           else
            return;
          else
           return;
         else
          if(p[offsets[8]] > cb)
           if(p[offsets[9]] > cb)
            if(p[offsets[10]] > cb)
             if(p[offsets[11]] > cb)
              if(p[offsets[12]] > cb)
               if(p[offsets[13]] > cb)
                if(p[offsets[14]] > cb)
                 if(p[offsets[15]] > cb)
                  {}
                 else
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[4]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[3]] > cb)
                if(p[offsets[4]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              if(p[offsets[2]] > cb)
               if(p[offsets[3]] > cb)
                if(p[offsets[4]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[7]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
            else
             return;
           else
            return;
          else if(p[offsets[8]] < c_b)
           if(p[offsets[7]] < c_b)
            if(p[offsets[9]] < c_b)
             if(p[offsets[10]] < c_b)
              if(p[offsets[6]] < c_b)
               if(p[offsets[5]] < c_b)
                if(p[offsets[4]] < c_b)
                 if(p[offsets[3]] < c_b)
                  if(p[offsets[2]] < c_b)
                   {}
                  else
                   if(p[offsets[11]] < c_b)
                    {}
                   else
                    return;
                 else
                  if(p[offsets[11]] < c_b)
                   if(p[offsets[12]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[11]] < c_b)
                  if(p[offsets[12]] < c_b)
                   if(p[offsets[13]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[11]] < c_b)
                 if(p[offsets[12]] < c_b)
                  if(p[offsets[13]] < c_b)
                   if(p[offsets[14]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[11]] < c_b)
                if(p[offsets[12]] < c_b)
                 if(p[offsets[13]] < c_b)
                  if(p[offsets[14]] < c_b)
                   if(p[offsets[15]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              return;
            else
             return;
           else
            return;
          else
           return;
        else if(p[offsets[0]] < c_b)
         if(p[offsets[1]] > cb)
          if(p[offsets[8]] > cb)
           if(p[offsets[7]] > cb)
            if(p[offsets[9]] > cb)
             if(p[offsets[6]] > cb)
              if(p[offsets[5]] > cb)
               if(p[offsets[4]] > cb)
                if(p[offsets[3]] > cb)
                 if(p[offsets[2]] > cb)
                  {}
                 else
                  if(p[offsets[10]] > cb)
                   if(p[offsets[11]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[10]] > cb)
                  if(p[offsets[11]] > cb)
                   if(p[offsets[12]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[10]] > cb)
                 if(p[offsets[11]] > cb)
                  if(p[offsets[12]] > cb)
                   if(p[offsets[13]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[10]] > cb)
                if(p[offsets[11]] > cb)
                 if(p[offsets[12]] > cb)
                  if(p[offsets[13]] > cb)
                   if(p[offsets[14]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              if(p[offsets[10]] > cb)
               if(p[offsets[11]] > cb)
                if(p[offsets[12]] > cb)
                 if(p[offsets[13]] > cb)
                  if(p[offsets[14]] > cb)
                   if(p[offsets[15]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
            else
             return;
           else
            return;
          else if(p[offsets[8]] < c_b)
           if(p[offsets[9]] < c_b)
            if(p[offsets[10]] < c_b)
             if(p[offsets[11]] < c_b)
              if(p[offsets[12]] < c_b)
               if(p[offsets[13]] < c_b)
                if(p[offsets[14]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[4]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[3]] < c_b)
                if(p[offsets[4]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              if(p[offsets[2]] < c_b)
               if(p[offsets[3]] < c_b)
                if(p[offsets[4]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
            else
             return;
           else
            return;
          else
           return;
         else if(p[offsets[1]] < c_b)
          if(p[offsets[2]] > cb)
           if(p[offsets[9]] > cb)
            if(p[offsets[7]] > cb)
             if(p[offsets[8]] > cb)
              if(p[offsets[10]] > cb)
               if(p[offsets[6]] > cb)
                if(p[offsets[5]] > cb)
                 if(p[offsets[4]] > cb)
                  if(p[offsets[3]] > cb)
                   {}
                  else
                   if(p[offsets[11]] > cb)
                    if(p[offsets[12]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                 else
                  if(p[offsets[11]] > cb)
                   if(p[offsets[12]] > cb)
                    if(p[offsets[13]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[11]] > cb)
                  if(p[offsets[12]] > cb)
                   if(p[offsets[13]] > cb)
                    if(p[offsets[14]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[11]] > cb)
                 if(p[offsets[12]] > cb)
                  if(p[offsets[13]] > cb)
                   if(p[offsets[14]] > cb)
                    if(p[offsets[15]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               return;
             else
              return;
            else
             return;
           else if(p[offsets[9]] < c_b)
            if(p[offsets[10]] < c_b)
             if(p[offsets[11]] < c_b)
              if(p[offsets[12]] < c_b)
               if(p[offsets[13]] < c_b)
                if(p[offsets[14]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[4]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[3]] < c_b)
                if(p[offsets[4]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              return;
            else
             return;
           else
            return;
          else if(p[offsets[2]] < c_b)
           if(p[offsets[3]] > cb)
            if(p[offsets[10]] > cb)
             if(p[offsets[7]] > cb)
              if(p[offsets[8]] > cb)
               if(p[offsets[9]] > cb)
                if(p[offsets[11]] > cb)
                 if(p[offsets[6]] > cb)
                  if(p[offsets[5]] > cb)
                   if(p[offsets[4]] > cb)
                    {}
                   else
                    if(p[offsets[12]] > cb)
                     if(p[offsets[13]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                  else
                   if(p[offsets[12]] > cb)
                    if(p[offsets[13]] > cb)
                     if(p[offsets[14]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                 else
                  if(p[offsets[12]] > cb)
                   if(p[offsets[13]] > cb)
                    if(p[offsets[14]] > cb)
                     if(p[offsets[15]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 return;
               else
                return;
              else
               return;
             else
              return;
            else if(p[offsets[10]] < c_b)
             if(p[offsets[11]] < c_b)
              if(p[offsets[12]] < c_b)
               if(p[offsets[13]] < c_b)
                if(p[offsets[14]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     if(p[offsets[9]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     if(p[offsets[9]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[4]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     if(p[offsets[9]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               return;
             else
              return;
            else
             return;
           else if(p[offsets[3]] < c_b)
            if(p[offsets[4]] > cb)
             if(p[offsets[13]] > cb)
              if(p[offsets[7]] > cb)
               if(p[offsets[8]] > cb)
                if(p[offsets[9]] > cb)
                 if(p[offsets[10]] > cb)
                  if(p[offsets[11]] > cb)
                   if(p[offsets[12]] > cb)
                    if(p[offsets[6]] > cb)
                     if(p[offsets[5]] > cb)
                      {}
                     else
                      if(p[offsets[14]] > cb)
                       {}
                      else
                       return;
                    else
                     if(p[offsets[14]] > cb)
                      if(p[offsets[15]] > cb)
                       {}
                      else
                       return;
                     else
                      return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
             else if(p[offsets[13]] < c_b)
              if(p[offsets[11]] > cb)
               if(p[offsets[5]] > cb)
                if(p[offsets[6]] > cb)
                 if(p[offsets[7]] > cb)
                  if(p[offsets[8]] > cb)
                   if(p[offsets[9]] > cb)
                    if(p[offsets[10]] > cb)
                     if(p[offsets[12]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else if(p[offsets[11]] < c_b)
               if(p[offsets[12]] < c_b)
                if(p[offsets[14]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     if(p[offsets[9]] < c_b)
                      if(p[offsets[10]] < c_b)
                       {}
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     if(p[offsets[9]] < c_b)
                      if(p[offsets[10]] < c_b)
                       {}
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                return;
              else
               return;
             else
              if(p[offsets[5]] > cb)
               if(p[offsets[6]] > cb)
                if(p[offsets[7]] > cb)
                 if(p[offsets[8]] > cb)
                  if(p[offsets[9]] > cb)
                   if(p[offsets[10]] > cb)
                    if(p[offsets[11]] > cb)
                     if(p[offsets[12]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
            else if(p[offsets[4]] < c_b)
             if(p[offsets[5]] > cb)
              if(p[offsets[14]] > cb)
               if(p[offsets[7]] > cb)
                if(p[offsets[8]] > cb)
                 if(p[offsets[9]] > cb)
                  if(p[offsets[10]] > cb)
                   if(p[offsets[11]] > cb)
                    if(p[offsets[12]] > cb)
                     if(p[offsets[13]] > cb)
                      if(p[offsets[6]] > cb)
                       {}
                      else
                       if(p[offsets[15]] > cb)
                        {}
                       else
                        return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else if(p[offsets[14]] < c_b)
               if(p[offsets[12]] > cb)
                if(p[offsets[6]] > cb)
                 if(p[offsets[7]] > cb)
                  if(p[offsets[8]] > cb)
                   if(p[offsets[9]] > cb)
                    if(p[offsets[10]] > cb)
                     if(p[offsets[11]] > cb)
                      if(p[offsets[13]] > cb)
                       {}
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else if(p[offsets[12]] < c_b)
                if(p[offsets[13]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     if(p[offsets[9]] < c_b)
                      if(p[offsets[10]] < c_b)
                       if(p[offsets[11]] < c_b)
                        {}
                       else
                        return;
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 return;
               else
                return;
              else
               if(p[offsets[6]] > cb)
                if(p[offsets[7]] > cb)
                 if(p[offsets[8]] > cb)
                  if(p[offsets[9]] > cb)
                   if(p[offsets[10]] > cb)
                    if(p[offsets[11]] > cb)
                     if(p[offsets[12]] > cb)
                      if(p[offsets[13]] > cb)
                       {}
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else if(p[offsets[5]] < c_b)
              if(p[offsets[6]] > cb)
               if(p[offsets[15]] < c_b)
                if(p[offsets[13]] > cb)
                 if(p[offsets[7]] > cb)
                  if(p[offsets[8]] > cb)
                   if(p[offsets[9]] > cb)
                    if(p[offsets[10]] > cb)
                     if(p[offsets[11]] > cb)
                      if(p[offsets[12]] > cb)
                       if(p[offsets[14]] > cb)
                        {}
                       else
                        return;
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else if(p[offsets[13]] < c_b)
                 if(p[offsets[14]] < c_b)
                  {}
                 else
                  return;
                else
                 return;
               else
                if(p[offsets[7]] > cb)
                 if(p[offsets[8]] > cb)
                  if(p[offsets[9]] > cb)
                   if(p[offsets[10]] > cb)
                    if(p[offsets[11]] > cb)
                     if(p[offsets[12]] > cb)
                      if(p[offsets[13]] > cb)
                       if(p[offsets[14]] > cb)
                        {}
                       else
                        return;
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else if(p[offsets[6]] < c_b)
               if(p[offsets[7]] > cb)
                if(p[offsets[14]] > cb)
                 if(p[offsets[8]] > cb)
                  if(p[offsets[9]] > cb)
                   if(p[offsets[10]] > cb)
                    if(p[offsets[11]] > cb)
                     if(p[offsets[12]] > cb)
                      if(p[offsets[13]] > cb)
                       if(p[offsets[15]] > cb)
                        {}
                       else
                        return;
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else if(p[offsets[14]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  return;
                else
                 return;
               else if(p[offsets[7]] < c_b)
                if(p[offsets[8]] < c_b)
                 {}
                else
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  return;
               else
                if(p[offsets[14]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[13]] > cb)
                if(p[offsets[7]] > cb)
                 if(p[offsets[8]] > cb)
                  if(p[offsets[9]] > cb)
                   if(p[offsets[10]] > cb)
                    if(p[offsets[11]] > cb)
                     if(p[offsets[12]] > cb)
                      if(p[offsets[14]] > cb)
                       if(p[offsets[15]] > cb)
                        {}
                       else
                        return;
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else if(p[offsets[13]] < c_b)
                if(p[offsets[14]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  return;
                else
                 return;
               else
                return;
             else
              if(p[offsets[12]] > cb)
               if(p[offsets[7]] > cb)
                if(p[offsets[8]] > cb)
                 if(p[offsets[9]] > cb)
                  if(p[offsets[10]] > cb)
                   if(p[offsets[11]] > cb)
                    if(p[offsets[13]] > cb)
                     if(p[offsets[14]] > cb)
                      if(p[offsets[6]] > cb)
                       {}
                      else
                       if(p[offsets[15]] > cb)
                        {}
                       else
                        return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else if(p[offsets[12]] < c_b)
               if(p[offsets[13]] < c_b)
                if(p[offsets[14]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     if(p[offsets[9]] < c_b)
                      if(p[offsets[10]] < c_b)
                       if(p[offsets[11]] < c_b)
                        {}
                       else
                        return;
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 return;
               else
                return;
              else
               return;
            else
             if(p[offsets[11]] > cb)
              if(p[offsets[7]] > cb)
               if(p[offsets[8]] > cb)
                if(p[offsets[9]] > cb)
                 if(p[offsets[10]] > cb)
                  if(p[offsets[12]] > cb)
                   if(p[offsets[13]] > cb)
                    if(p[offsets[6]] > cb)
                     if(p[offsets[5]] > cb)
                      {}
                     else
                      if(p[offsets[14]] > cb)
                       {}
                      else
                       return;
                    else
                     if(p[offsets[14]] > cb)
                      if(p[offsets[15]] > cb)
                       {}
                      else
                       return;
                     else
                      return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
             else if(p[offsets[11]] < c_b)
              if(p[offsets[12]] < c_b)
               if(p[offsets[13]] < c_b)
                if(p[offsets[14]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     if(p[offsets[9]] < c_b)
                      if(p[offsets[10]] < c_b)
                       {}
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     if(p[offsets[9]] < c_b)
                      if(p[offsets[10]] < c_b)
                       {}
                      else
                       return;
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                return;
              else
               return;
             else
              return;
           else
            if(p[offsets[10]] > cb)
             if(p[offsets[7]] > cb)
              if(p[offsets[8]] > cb)
               if(p[offsets[9]] > cb)
                if(p[offsets[11]] > cb)
                 if(p[offsets[12]] > cb)
                  if(p[offsets[6]] > cb)
                   if(p[offsets[5]] > cb)
                    if(p[offsets[4]] > cb)
                     {}
                    else
                     if(p[offsets[13]] > cb)
                      {}
                     else
                      return;
                   else
                    if(p[offsets[13]] > cb)
                     if(p[offsets[14]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                  else
                   if(p[offsets[13]] > cb)
                    if(p[offsets[14]] > cb)
                     if(p[offsets[15]] > cb)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
             else
              return;
            else if(p[offsets[10]] < c_b)
             if(p[offsets[11]] < c_b)
              if(p[offsets[12]] < c_b)
               if(p[offsets[13]] < c_b)
                if(p[offsets[14]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     if(p[offsets[9]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     if(p[offsets[9]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[4]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     if(p[offsets[9]] < c_b)
                      {}
                     else
                      return;
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               return;
             else
              return;
            else
             return;
          else
           if(p[offsets[9]] > cb)
            if(p[offsets[7]] > cb)
             if(p[offsets[8]] > cb)
              if(p[offsets[10]] > cb)
               if(p[offsets[11]] > cb)
                if(p[offsets[6]] > cb)
                 if(p[offsets[5]] > cb)
                  if(p[offsets[4]] > cb)
                   if(p[offsets[3]] > cb)
                    {}
                   else
                    if(p[offsets[12]] > cb)
                     {}
                    else
                     return;
                  else
                   if(p[offsets[12]] > cb)
                    if(p[offsets[13]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                 else
                  if(p[offsets[12]] > cb)
                   if(p[offsets[13]] > cb)
                    if(p[offsets[14]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[12]] > cb)
                  if(p[offsets[13]] > cb)
                   if(p[offsets[14]] > cb)
                    if(p[offsets[15]] > cb)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                return;
              else
               return;
             else
              return;
            else
             return;
           else if(p[offsets[9]] < c_b)
            if(p[offsets[10]] < c_b)
             if(p[offsets[11]] < c_b)
              if(p[offsets[12]] < c_b)
               if(p[offsets[13]] < c_b)
                if(p[offsets[14]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[4]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[3]] < c_b)
                if(p[offsets[4]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    if(p[offsets[8]] < c_b)
                     {}
                    else
                     return;
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              return;
            else
             return;
           else
            return;
         else
          if(p[offsets[8]] > cb)
           if(p[offsets[7]] > cb)
            if(p[offsets[9]] > cb)
             if(p[offsets[10]] > cb)
              if(p[offsets[6]] > cb)
               if(p[offsets[5]] > cb)
                if(p[offsets[4]] > cb)
                 if(p[offsets[3]] > cb)
                  if(p[offsets[2]] > cb)
                   {}
                  else
                   if(p[offsets[11]] > cb)
                    {}
                   else
                    return;
                 else
                  if(p[offsets[11]] > cb)
                   if(p[offsets[12]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[11]] > cb)
                  if(p[offsets[12]] > cb)
                   if(p[offsets[13]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[11]] > cb)
                 if(p[offsets[12]] > cb)
                  if(p[offsets[13]] > cb)
                   if(p[offsets[14]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[11]] > cb)
                if(p[offsets[12]] > cb)
                 if(p[offsets[13]] > cb)
                  if(p[offsets[14]] > cb)
                   if(p[offsets[15]] > cb)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              return;
            else
             return;
           else
            return;
          else if(p[offsets[8]] < c_b)
           if(p[offsets[9]] < c_b)
            if(p[offsets[10]] < c_b)
             if(p[offsets[11]] < c_b)
              if(p[offsets[12]] < c_b)
               if(p[offsets[13]] < c_b)
                if(p[offsets[14]] < c_b)
                 if(p[offsets[15]] < c_b)
                  {}
                 else
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                else
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[4]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[3]] < c_b)
                if(p[offsets[4]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              if(p[offsets[2]] < c_b)
               if(p[offsets[3]] < c_b)
                if(p[offsets[4]] < c_b)
                 if(p[offsets[5]] < c_b)
                  if(p[offsets[6]] < c_b)
                   if(p[offsets[7]] < c_b)
                    {}
                   else
                    return;
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
            else
             return;
           else
            return;
          else
           return;
        else
         if(p[offsets[7]] > cb)
          if(p[offsets[8]] > cb)
           if(p[offsets[9]] > cb)
            if(p[offsets[6]] > cb)
             if(p[offsets[5]] > cb)
              if(p[offsets[4]] > cb)
               if(p[offsets[3]] > cb)
                if(p[offsets[2]] > cb)
                 if(p[offsets[1]] > cb)
                  {}
                 else
                  if(p[offsets[10]] > cb)
                   {}
                  else
                   return;
                else
                 if(p[offsets[10]] > cb)
                  if(p[offsets[11]] > cb)
                   {}
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[10]] > cb)
                 if(p[offsets[11]] > cb)
                  if(p[offsets[12]] > cb)
                   {}
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[10]] > cb)
                if(p[offsets[11]] > cb)
                 if(p[offsets[12]] > cb)
                  if(p[offsets[13]] > cb)
                   {}
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              if(p[offsets[10]] > cb)
               if(p[offsets[11]] > cb)
                if(p[offsets[12]] > cb)
                 if(p[offsets[13]] > cb)
                  if(p[offsets[14]] > cb)
                   {}
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
            else
             if(p[offsets[10]] > cb)
              if(p[offsets[11]] > cb)
               if(p[offsets[12]] > cb)
                if(p[offsets[13]] > cb)
                 if(p[offsets[14]] > cb)
                  if(p[offsets[15]] > cb)
                   {}
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
             else
              return;
           else
            return;
          else
           return;
         else if(p[offsets[7]] < c_b)
          if(p[offsets[8]] < c_b)
           if(p[offsets[9]] < c_b)
            if(p[offsets[6]] < c_b)
             if(p[offsets[5]] < c_b)
              if(p[offsets[4]] < c_b)
               if(p[offsets[3]] < c_b)
                if(p[offsets[2]] < c_b)
                 if(p[offsets[1]] < c_b)
                  {}
                 else
                  if(p[offsets[10]] < c_b)
                   {}
                  else
                   return;
                else
                 if(p[offsets[10]] < c_b)
                  if(p[offsets[11]] < c_b)
                   {}
                  else
                   return;
                 else
                  return;
               else
                if(p[offsets[10]] < c_b)
                 if(p[offsets[11]] < c_b)
                  if(p[offsets[12]] < c_b)
                   {}
                  else
                   return;
                 else
                  return;
                else
                 return;
              else
               if(p[offsets[10]] < c_b)
                if(p[offsets[11]] < c_b)
                 if(p[offsets[12]] < c_b)
                  if(p[offsets[13]] < c_b)
                   {}
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
             else
              if(p[offsets[10]] < c_b)
               if(p[offsets[11]] < c_b)
                if(p[offsets[12]] < c_b)
                 if(p[offsets[13]] < c_b)
                  if(p[offsets[14]] < c_b)
                   {}
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
            else
             if(p[offsets[10]] < c_b)
              if(p[offsets[11]] < c_b)
               if(p[offsets[12]] < c_b)
                if(p[offsets[13]] < c_b)
                 if(p[offsets[14]] < c_b)
                  if(p[offsets[15]] < c_b)
                   {}
                  else
                   return;
                 else
                  return;
                else
                 return;
               else
                return;
              else
               return;
             else
              return;
           else
            return;
          else
           return;
         else
          return;

    //SCORE COUNT
    uint32_t bmin = FAST_THRESHOLD;
    uint32_t bmax = 255;
    uint32_t b = (bmax + bmin)/2;
    cb = *p + b;
    c_b= *p - b;

    for(;;)
    {
        if( p[offsets[0]] > cb)
         if( p[offsets[1]] > cb)
          if( p[offsets[2]] > cb)
           if( p[offsets[3]] > cb)
            if( p[offsets[4]] > cb)
             if( p[offsets[5]] > cb)
              if( p[offsets[6]] > cb)
               if( p[offsets[7]] > cb)
                if( p[offsets[8]] > cb)
                 goto is_a_corner;
                else
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
               else if( p[offsets[7]] < c_b)
                if( p[offsets[14]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else if( p[offsets[14]] < c_b)
                 if( p[offsets[8]] < c_b)
                  if( p[offsets[9]] < c_b)
                   if( p[offsets[10]] < c_b)
                    if( p[offsets[11]] < c_b)
                     if( p[offsets[12]] < c_b)
                      if( p[offsets[13]] < c_b)
                       if( p[offsets[15]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                if( p[offsets[14]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else if( p[offsets[6]] < c_b)
               if( p[offsets[15]] > cb)
                if( p[offsets[13]] > cb)
                 if( p[offsets[14]] > cb)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else if( p[offsets[13]] < c_b)
                 if( p[offsets[7]] < c_b)
                  if( p[offsets[8]] < c_b)
                   if( p[offsets[9]] < c_b)
                    if( p[offsets[10]] < c_b)
                     if( p[offsets[11]] < c_b)
                      if( p[offsets[12]] < c_b)
                       if( p[offsets[14]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                if( p[offsets[7]] < c_b)
                 if( p[offsets[8]] < c_b)
                  if( p[offsets[9]] < c_b)
                   if( p[offsets[10]] < c_b)
                    if( p[offsets[11]] < c_b)
                     if( p[offsets[12]] < c_b)
                      if( p[offsets[13]] < c_b)
                       if( p[offsets[14]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[13]] > cb)
                if( p[offsets[14]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else if( p[offsets[13]] < c_b)
                if( p[offsets[7]] < c_b)
                 if( p[offsets[8]] < c_b)
                  if( p[offsets[9]] < c_b)
                   if( p[offsets[10]] < c_b)
                    if( p[offsets[11]] < c_b)
                     if( p[offsets[12]] < c_b)
                      if( p[offsets[14]] < c_b)
                       if( p[offsets[15]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else if( p[offsets[5]] < c_b)
              if( p[offsets[14]] > cb)
               if( p[offsets[12]] > cb)
                if( p[offsets[13]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     if( p[offsets[9]] > cb)
                      if( p[offsets[10]] > cb)
                       if( p[offsets[11]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else if( p[offsets[12]] < c_b)
                if( p[offsets[6]] < c_b)
                 if( p[offsets[7]] < c_b)
                  if( p[offsets[8]] < c_b)
                   if( p[offsets[9]] < c_b)
                    if( p[offsets[10]] < c_b)
                     if( p[offsets[11]] < c_b)
                      if( p[offsets[13]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else if( p[offsets[14]] < c_b)
               if( p[offsets[7]] < c_b)
                if( p[offsets[8]] < c_b)
                 if( p[offsets[9]] < c_b)
                  if( p[offsets[10]] < c_b)
                   if( p[offsets[11]] < c_b)
                    if( p[offsets[12]] < c_b)
                     if( p[offsets[13]] < c_b)
                      if( p[offsets[6]] < c_b)
                       goto is_a_corner;
                      else
                       if( p[offsets[15]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               if( p[offsets[6]] < c_b)
                if( p[offsets[7]] < c_b)
                 if( p[offsets[8]] < c_b)
                  if( p[offsets[9]] < c_b)
                   if( p[offsets[10]] < c_b)
                    if( p[offsets[11]] < c_b)
                     if( p[offsets[12]] < c_b)
                      if( p[offsets[13]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[offsets[12]] > cb)
               if( p[offsets[13]] > cb)
                if( p[offsets[14]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     if( p[offsets[9]] > cb)
                      if( p[offsets[10]] > cb)
                       if( p[offsets[11]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else if( p[offsets[12]] < c_b)
               if( p[offsets[7]] < c_b)
                if( p[offsets[8]] < c_b)
                 if( p[offsets[9]] < c_b)
                  if( p[offsets[10]] < c_b)
                   if( p[offsets[11]] < c_b)
                    if( p[offsets[13]] < c_b)
                     if( p[offsets[14]] < c_b)
                      if( p[offsets[6]] < c_b)
                       goto is_a_corner;
                      else
                       if( p[offsets[15]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else if( p[offsets[4]] < c_b)
             if( p[offsets[13]] > cb)
              if( p[offsets[11]] > cb)
               if( p[offsets[12]] > cb)
                if( p[offsets[14]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     if( p[offsets[9]] > cb)
                      if( p[offsets[10]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     if( p[offsets[9]] > cb)
                      if( p[offsets[10]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else if( p[offsets[11]] < c_b)
               if( p[offsets[5]] < c_b)
                if( p[offsets[6]] < c_b)
                 if( p[offsets[7]] < c_b)
                  if( p[offsets[8]] < c_b)
                   if( p[offsets[9]] < c_b)
                    if( p[offsets[10]] < c_b)
                     if( p[offsets[12]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else if( p[offsets[13]] < c_b)
              if( p[offsets[7]] < c_b)
               if( p[offsets[8]] < c_b)
                if( p[offsets[9]] < c_b)
                 if( p[offsets[10]] < c_b)
                  if( p[offsets[11]] < c_b)
                   if( p[offsets[12]] < c_b)
                    if( p[offsets[6]] < c_b)
                     if( p[offsets[5]] < c_b)
                      goto is_a_corner;
                     else
                      if( p[offsets[14]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                    else
                     if( p[offsets[14]] < c_b)
                      if( p[offsets[15]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              if( p[offsets[5]] < c_b)
               if( p[offsets[6]] < c_b)
                if( p[offsets[7]] < c_b)
                 if( p[offsets[8]] < c_b)
                  if( p[offsets[9]] < c_b)
                   if( p[offsets[10]] < c_b)
                    if( p[offsets[11]] < c_b)
                     if( p[offsets[12]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             if( p[offsets[11]] > cb)
              if( p[offsets[12]] > cb)
               if( p[offsets[13]] > cb)
                if( p[offsets[14]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     if( p[offsets[9]] > cb)
                      if( p[offsets[10]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     if( p[offsets[9]] > cb)
                      if( p[offsets[10]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else if( p[offsets[11]] < c_b)
              if( p[offsets[7]] < c_b)
               if( p[offsets[8]] < c_b)
                if( p[offsets[9]] < c_b)
                 if( p[offsets[10]] < c_b)
                  if( p[offsets[12]] < c_b)
                   if( p[offsets[13]] < c_b)
                    if( p[offsets[6]] < c_b)
                     if( p[offsets[5]] < c_b)
                      goto is_a_corner;
                     else
                      if( p[offsets[14]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                    else
                     if( p[offsets[14]] < c_b)
                      if( p[offsets[15]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
           else if( p[offsets[3]] < c_b)
            if( p[offsets[10]] > cb)
             if( p[offsets[11]] > cb)
              if( p[offsets[12]] > cb)
               if( p[offsets[13]] > cb)
                if( p[offsets[14]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     if( p[offsets[9]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     if( p[offsets[9]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[4]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     if( p[offsets[9]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else if( p[offsets[10]] < c_b)
             if( p[offsets[7]] < c_b)
              if( p[offsets[8]] < c_b)
               if( p[offsets[9]] < c_b)
                if( p[offsets[11]] < c_b)
                 if( p[offsets[6]] < c_b)
                  if( p[offsets[5]] < c_b)
                   if( p[offsets[4]] < c_b)
                    goto is_a_corner;
                   else
                    if( p[offsets[12]] < c_b)
                     if( p[offsets[13]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                  else
                   if( p[offsets[12]] < c_b)
                    if( p[offsets[13]] < c_b)
                     if( p[offsets[14]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[offsets[12]] < c_b)
                   if( p[offsets[13]] < c_b)
                    if( p[offsets[14]] < c_b)
                     if( p[offsets[15]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            if( p[offsets[10]] > cb)
             if( p[offsets[11]] > cb)
              if( p[offsets[12]] > cb)
               if( p[offsets[13]] > cb)
                if( p[offsets[14]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     if( p[offsets[9]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     if( p[offsets[9]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[4]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     if( p[offsets[9]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else if( p[offsets[10]] < c_b)
             if( p[offsets[7]] < c_b)
              if( p[offsets[8]] < c_b)
               if( p[offsets[9]] < c_b)
                if( p[offsets[11]] < c_b)
                 if( p[offsets[12]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[5]] < c_b)
                    if( p[offsets[4]] < c_b)
                     goto is_a_corner;
                    else
                     if( p[offsets[13]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                   else
                    if( p[offsets[13]] < c_b)
                     if( p[offsets[14]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                  else
                   if( p[offsets[13]] < c_b)
                    if( p[offsets[14]] < c_b)
                     if( p[offsets[15]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
          else if( p[offsets[2]] < c_b)
           if( p[offsets[9]] > cb)
            if( p[offsets[10]] > cb)
             if( p[offsets[11]] > cb)
              if( p[offsets[12]] > cb)
               if( p[offsets[13]] > cb)
                if( p[offsets[14]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[4]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[3]] > cb)
                if( p[offsets[4]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else if( p[offsets[9]] < c_b)
            if( p[offsets[7]] < c_b)
             if( p[offsets[8]] < c_b)
              if( p[offsets[10]] < c_b)
               if( p[offsets[6]] < c_b)
                if( p[offsets[5]] < c_b)
                 if( p[offsets[4]] < c_b)
                  if( p[offsets[3]] < c_b)
                   goto is_a_corner;
                  else
                   if( p[offsets[11]] < c_b)
                    if( p[offsets[12]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[offsets[11]] < c_b)
                   if( p[offsets[12]] < c_b)
                    if( p[offsets[13]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[11]] < c_b)
                  if( p[offsets[12]] < c_b)
                   if( p[offsets[13]] < c_b)
                    if( p[offsets[14]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[11]] < c_b)
                 if( p[offsets[12]] < c_b)
                  if( p[offsets[13]] < c_b)
                   if( p[offsets[14]] < c_b)
                    if( p[offsets[15]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           if( p[offsets[9]] > cb)
            if( p[offsets[10]] > cb)
             if( p[offsets[11]] > cb)
              if( p[offsets[12]] > cb)
               if( p[offsets[13]] > cb)
                if( p[offsets[14]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[4]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[3]] > cb)
                if( p[offsets[4]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    if( p[offsets[8]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else if( p[offsets[9]] < c_b)
            if( p[offsets[7]] < c_b)
             if( p[offsets[8]] < c_b)
              if( p[offsets[10]] < c_b)
               if( p[offsets[11]] < c_b)
                if( p[offsets[6]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[4]] < c_b)
                   if( p[offsets[3]] < c_b)
                    goto is_a_corner;
                   else
                    if( p[offsets[12]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                  else
                   if( p[offsets[12]] < c_b)
                    if( p[offsets[13]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[offsets[12]] < c_b)
                   if( p[offsets[13]] < c_b)
                    if( p[offsets[14]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[12]] < c_b)
                  if( p[offsets[13]] < c_b)
                   if( p[offsets[14]] < c_b)
                    if( p[offsets[15]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
         else if( p[offsets[1]] < c_b)
          if( p[offsets[8]] > cb)
           if( p[offsets[9]] > cb)
            if( p[offsets[10]] > cb)
             if( p[offsets[11]] > cb)
              if( p[offsets[12]] > cb)
               if( p[offsets[13]] > cb)
                if( p[offsets[14]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[4]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[3]] > cb)
                if( p[offsets[4]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[offsets[2]] > cb)
               if( p[offsets[3]] > cb)
                if( p[offsets[4]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else if( p[offsets[8]] < c_b)
           if( p[offsets[7]] < c_b)
            if( p[offsets[9]] < c_b)
             if( p[offsets[6]] < c_b)
              if( p[offsets[5]] < c_b)
               if( p[offsets[4]] < c_b)
                if( p[offsets[3]] < c_b)
                 if( p[offsets[2]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[offsets[10]] < c_b)
                   if( p[offsets[11]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[10]] < c_b)
                  if( p[offsets[11]] < c_b)
                   if( p[offsets[12]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[10]] < c_b)
                 if( p[offsets[11]] < c_b)
                  if( p[offsets[12]] < c_b)
                   if( p[offsets[13]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[10]] < c_b)
                if( p[offsets[11]] < c_b)
                 if( p[offsets[12]] < c_b)
                  if( p[offsets[13]] < c_b)
                   if( p[offsets[14]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[offsets[10]] < c_b)
               if( p[offsets[11]] < c_b)
                if( p[offsets[12]] < c_b)
                 if( p[offsets[13]] < c_b)
                  if( p[offsets[14]] < c_b)
                   if( p[offsets[15]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           goto is_not_a_corner;
         else
          if( p[offsets[8]] > cb)
           if( p[offsets[9]] > cb)
            if( p[offsets[10]] > cb)
             if( p[offsets[11]] > cb)
              if( p[offsets[12]] > cb)
               if( p[offsets[13]] > cb)
                if( p[offsets[14]] > cb)
                 if( p[offsets[15]] > cb)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[4]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[3]] > cb)
                if( p[offsets[4]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[offsets[2]] > cb)
               if( p[offsets[3]] > cb)
                if( p[offsets[4]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[7]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else if( p[offsets[8]] < c_b)
           if( p[offsets[7]] < c_b)
            if( p[offsets[9]] < c_b)
             if( p[offsets[10]] < c_b)
              if( p[offsets[6]] < c_b)
               if( p[offsets[5]] < c_b)
                if( p[offsets[4]] < c_b)
                 if( p[offsets[3]] < c_b)
                  if( p[offsets[2]] < c_b)
                   goto is_a_corner;
                  else
                   if( p[offsets[11]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[offsets[11]] < c_b)
                   if( p[offsets[12]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[11]] < c_b)
                  if( p[offsets[12]] < c_b)
                   if( p[offsets[13]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[11]] < c_b)
                 if( p[offsets[12]] < c_b)
                  if( p[offsets[13]] < c_b)
                   if( p[offsets[14]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[11]] < c_b)
                if( p[offsets[12]] < c_b)
                 if( p[offsets[13]] < c_b)
                  if( p[offsets[14]] < c_b)
                   if( p[offsets[15]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           goto is_not_a_corner;
        else if( p[offsets[0]] < c_b)
         if( p[offsets[1]] > cb)
          if( p[offsets[8]] > cb)
           if( p[offsets[7]] > cb)
            if( p[offsets[9]] > cb)
             if( p[offsets[6]] > cb)
              if( p[offsets[5]] > cb)
               if( p[offsets[4]] > cb)
                if( p[offsets[3]] > cb)
                 if( p[offsets[2]] > cb)
                  goto is_a_corner;
                 else
                  if( p[offsets[10]] > cb)
                   if( p[offsets[11]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[10]] > cb)
                  if( p[offsets[11]] > cb)
                   if( p[offsets[12]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[10]] > cb)
                 if( p[offsets[11]] > cb)
                  if( p[offsets[12]] > cb)
                   if( p[offsets[13]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[10]] > cb)
                if( p[offsets[11]] > cb)
                 if( p[offsets[12]] > cb)
                  if( p[offsets[13]] > cb)
                   if( p[offsets[14]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[offsets[10]] > cb)
               if( p[offsets[11]] > cb)
                if( p[offsets[12]] > cb)
                 if( p[offsets[13]] > cb)
                  if( p[offsets[14]] > cb)
                   if( p[offsets[15]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else if( p[offsets[8]] < c_b)
           if( p[offsets[9]] < c_b)
            if( p[offsets[10]] < c_b)
             if( p[offsets[11]] < c_b)
              if( p[offsets[12]] < c_b)
               if( p[offsets[13]] < c_b)
                if( p[offsets[14]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[4]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[3]] < c_b)
                if( p[offsets[4]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[offsets[2]] < c_b)
               if( p[offsets[3]] < c_b)
                if( p[offsets[4]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           goto is_not_a_corner;
         else if( p[offsets[1]] < c_b)
          if( p[offsets[2]] > cb)
           if( p[offsets[9]] > cb)
            if( p[offsets[7]] > cb)
             if( p[offsets[8]] > cb)
              if( p[offsets[10]] > cb)
               if( p[offsets[6]] > cb)
                if( p[offsets[5]] > cb)
                 if( p[offsets[4]] > cb)
                  if( p[offsets[3]] > cb)
                   goto is_a_corner;
                  else
                   if( p[offsets[11]] > cb)
                    if( p[offsets[12]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[offsets[11]] > cb)
                   if( p[offsets[12]] > cb)
                    if( p[offsets[13]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[11]] > cb)
                  if( p[offsets[12]] > cb)
                   if( p[offsets[13]] > cb)
                    if( p[offsets[14]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[11]] > cb)
                 if( p[offsets[12]] > cb)
                  if( p[offsets[13]] > cb)
                   if( p[offsets[14]] > cb)
                    if( p[offsets[15]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else if( p[offsets[9]] < c_b)
            if( p[offsets[10]] < c_b)
             if( p[offsets[11]] < c_b)
              if( p[offsets[12]] < c_b)
               if( p[offsets[13]] < c_b)
                if( p[offsets[14]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[4]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[3]] < c_b)
                if( p[offsets[4]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else if( p[offsets[2]] < c_b)
           if( p[offsets[3]] > cb)
            if( p[offsets[10]] > cb)
             if( p[offsets[7]] > cb)
              if( p[offsets[8]] > cb)
               if( p[offsets[9]] > cb)
                if( p[offsets[11]] > cb)
                 if( p[offsets[6]] > cb)
                  if( p[offsets[5]] > cb)
                   if( p[offsets[4]] > cb)
                    goto is_a_corner;
                   else
                    if( p[offsets[12]] > cb)
                     if( p[offsets[13]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                  else
                   if( p[offsets[12]] > cb)
                    if( p[offsets[13]] > cb)
                     if( p[offsets[14]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[offsets[12]] > cb)
                   if( p[offsets[13]] > cb)
                    if( p[offsets[14]] > cb)
                     if( p[offsets[15]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else if( p[offsets[10]] < c_b)
             if( p[offsets[11]] < c_b)
              if( p[offsets[12]] < c_b)
               if( p[offsets[13]] < c_b)
                if( p[offsets[14]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     if( p[offsets[9]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     if( p[offsets[9]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[4]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     if( p[offsets[9]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else if( p[offsets[3]] < c_b)
            if( p[offsets[4]] > cb)
             if( p[offsets[13]] > cb)
              if( p[offsets[7]] > cb)
               if( p[offsets[8]] > cb)
                if( p[offsets[9]] > cb)
                 if( p[offsets[10]] > cb)
                  if( p[offsets[11]] > cb)
                   if( p[offsets[12]] > cb)
                    if( p[offsets[6]] > cb)
                     if( p[offsets[5]] > cb)
                      goto is_a_corner;
                     else
                      if( p[offsets[14]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                    else
                     if( p[offsets[14]] > cb)
                      if( p[offsets[15]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else if( p[offsets[13]] < c_b)
              if( p[offsets[11]] > cb)
               if( p[offsets[5]] > cb)
                if( p[offsets[6]] > cb)
                 if( p[offsets[7]] > cb)
                  if( p[offsets[8]] > cb)
                   if( p[offsets[9]] > cb)
                    if( p[offsets[10]] > cb)
                     if( p[offsets[12]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else if( p[offsets[11]] < c_b)
               if( p[offsets[12]] < c_b)
                if( p[offsets[14]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     if( p[offsets[9]] < c_b)
                      if( p[offsets[10]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     if( p[offsets[9]] < c_b)
                      if( p[offsets[10]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              if( p[offsets[5]] > cb)
               if( p[offsets[6]] > cb)
                if( p[offsets[7]] > cb)
                 if( p[offsets[8]] > cb)
                  if( p[offsets[9]] > cb)
                   if( p[offsets[10]] > cb)
                    if( p[offsets[11]] > cb)
                     if( p[offsets[12]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else if( p[offsets[4]] < c_b)
             if( p[offsets[5]] > cb)
              if( p[offsets[14]] > cb)
               if( p[offsets[7]] > cb)
                if( p[offsets[8]] > cb)
                 if( p[offsets[9]] > cb)
                  if( p[offsets[10]] > cb)
                   if( p[offsets[11]] > cb)
                    if( p[offsets[12]] > cb)
                     if( p[offsets[13]] > cb)
                      if( p[offsets[6]] > cb)
                       goto is_a_corner;
                      else
                       if( p[offsets[15]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else if( p[offsets[14]] < c_b)
               if( p[offsets[12]] > cb)
                if( p[offsets[6]] > cb)
                 if( p[offsets[7]] > cb)
                  if( p[offsets[8]] > cb)
                   if( p[offsets[9]] > cb)
                    if( p[offsets[10]] > cb)
                     if( p[offsets[11]] > cb)
                      if( p[offsets[13]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else if( p[offsets[12]] < c_b)
                if( p[offsets[13]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     if( p[offsets[9]] < c_b)
                      if( p[offsets[10]] < c_b)
                       if( p[offsets[11]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               if( p[offsets[6]] > cb)
                if( p[offsets[7]] > cb)
                 if( p[offsets[8]] > cb)
                  if( p[offsets[9]] > cb)
                   if( p[offsets[10]] > cb)
                    if( p[offsets[11]] > cb)
                     if( p[offsets[12]] > cb)
                      if( p[offsets[13]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else if( p[offsets[5]] < c_b)
              if( p[offsets[6]] > cb)
               if( p[offsets[15]] < c_b)
                if( p[offsets[13]] > cb)
                 if( p[offsets[7]] > cb)
                  if( p[offsets[8]] > cb)
                   if( p[offsets[9]] > cb)
                    if( p[offsets[10]] > cb)
                     if( p[offsets[11]] > cb)
                      if( p[offsets[12]] > cb)
                       if( p[offsets[14]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else if( p[offsets[13]] < c_b)
                 if( p[offsets[14]] < c_b)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                if( p[offsets[7]] > cb)
                 if( p[offsets[8]] > cb)
                  if( p[offsets[9]] > cb)
                   if( p[offsets[10]] > cb)
                    if( p[offsets[11]] > cb)
                     if( p[offsets[12]] > cb)
                      if( p[offsets[13]] > cb)
                       if( p[offsets[14]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else if( p[offsets[6]] < c_b)
               if( p[offsets[7]] > cb)
                if( p[offsets[14]] > cb)
                 if( p[offsets[8]] > cb)
                  if( p[offsets[9]] > cb)
                   if( p[offsets[10]] > cb)
                    if( p[offsets[11]] > cb)
                     if( p[offsets[12]] > cb)
                      if( p[offsets[13]] > cb)
                       if( p[offsets[15]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else if( p[offsets[14]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else if( p[offsets[7]] < c_b)
                if( p[offsets[8]] < c_b)
                 goto is_a_corner;
                else
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[14]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[13]] > cb)
                if( p[offsets[7]] > cb)
                 if( p[offsets[8]] > cb)
                  if( p[offsets[9]] > cb)
                   if( p[offsets[10]] > cb)
                    if( p[offsets[11]] > cb)
                     if( p[offsets[12]] > cb)
                      if( p[offsets[14]] > cb)
                       if( p[offsets[15]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else if( p[offsets[13]] < c_b)
                if( p[offsets[14]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[offsets[12]] > cb)
               if( p[offsets[7]] > cb)
                if( p[offsets[8]] > cb)
                 if( p[offsets[9]] > cb)
                  if( p[offsets[10]] > cb)
                   if( p[offsets[11]] > cb)
                    if( p[offsets[13]] > cb)
                     if( p[offsets[14]] > cb)
                      if( p[offsets[6]] > cb)
                       goto is_a_corner;
                      else
                       if( p[offsets[15]] > cb)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else if( p[offsets[12]] < c_b)
               if( p[offsets[13]] < c_b)
                if( p[offsets[14]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     if( p[offsets[9]] < c_b)
                      if( p[offsets[10]] < c_b)
                       if( p[offsets[11]] < c_b)
                        goto is_a_corner;
                       else
                        goto is_not_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             if( p[offsets[11]] > cb)
              if( p[offsets[7]] > cb)
               if( p[offsets[8]] > cb)
                if( p[offsets[9]] > cb)
                 if( p[offsets[10]] > cb)
                  if( p[offsets[12]] > cb)
                   if( p[offsets[13]] > cb)
                    if( p[offsets[6]] > cb)
                     if( p[offsets[5]] > cb)
                      goto is_a_corner;
                     else
                      if( p[offsets[14]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                    else
                     if( p[offsets[14]] > cb)
                      if( p[offsets[15]] > cb)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else if( p[offsets[11]] < c_b)
              if( p[offsets[12]] < c_b)
               if( p[offsets[13]] < c_b)
                if( p[offsets[14]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     if( p[offsets[9]] < c_b)
                      if( p[offsets[10]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     if( p[offsets[9]] < c_b)
                      if( p[offsets[10]] < c_b)
                       goto is_a_corner;
                      else
                       goto is_not_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
           else
            if( p[offsets[10]] > cb)
             if( p[offsets[7]] > cb)
              if( p[offsets[8]] > cb)
               if( p[offsets[9]] > cb)
                if( p[offsets[11]] > cb)
                 if( p[offsets[12]] > cb)
                  if( p[offsets[6]] > cb)
                   if( p[offsets[5]] > cb)
                    if( p[offsets[4]] > cb)
                     goto is_a_corner;
                    else
                     if( p[offsets[13]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                   else
                    if( p[offsets[13]] > cb)
                     if( p[offsets[14]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                  else
                   if( p[offsets[13]] > cb)
                    if( p[offsets[14]] > cb)
                     if( p[offsets[15]] > cb)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else if( p[offsets[10]] < c_b)
             if( p[offsets[11]] < c_b)
              if( p[offsets[12]] < c_b)
               if( p[offsets[13]] < c_b)
                if( p[offsets[14]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     if( p[offsets[9]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     if( p[offsets[9]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[4]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     if( p[offsets[9]] < c_b)
                      goto is_a_corner;
                     else
                      goto is_not_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
          else
           if( p[offsets[9]] > cb)
            if( p[offsets[7]] > cb)
             if( p[offsets[8]] > cb)
              if( p[offsets[10]] > cb)
               if( p[offsets[11]] > cb)
                if( p[offsets[6]] > cb)
                 if( p[offsets[5]] > cb)
                  if( p[offsets[4]] > cb)
                   if( p[offsets[3]] > cb)
                    goto is_a_corner;
                   else
                    if( p[offsets[12]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                  else
                   if( p[offsets[12]] > cb)
                    if( p[offsets[13]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[offsets[12]] > cb)
                   if( p[offsets[13]] > cb)
                    if( p[offsets[14]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[12]] > cb)
                  if( p[offsets[13]] > cb)
                   if( p[offsets[14]] > cb)
                    if( p[offsets[15]] > cb)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else if( p[offsets[9]] < c_b)
            if( p[offsets[10]] < c_b)
             if( p[offsets[11]] < c_b)
              if( p[offsets[12]] < c_b)
               if( p[offsets[13]] < c_b)
                if( p[offsets[14]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[4]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[3]] < c_b)
                if( p[offsets[4]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    if( p[offsets[8]] < c_b)
                     goto is_a_corner;
                    else
                     goto is_not_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
         else
          if( p[offsets[8]] > cb)
           if( p[offsets[7]] > cb)
            if( p[offsets[9]] > cb)
             if( p[offsets[10]] > cb)
              if( p[offsets[6]] > cb)
               if( p[offsets[5]] > cb)
                if( p[offsets[4]] > cb)
                 if( p[offsets[3]] > cb)
                  if( p[offsets[2]] > cb)
                   goto is_a_corner;
                  else
                   if( p[offsets[11]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                 else
                  if( p[offsets[11]] > cb)
                   if( p[offsets[12]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[11]] > cb)
                  if( p[offsets[12]] > cb)
                   if( p[offsets[13]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[11]] > cb)
                 if( p[offsets[12]] > cb)
                  if( p[offsets[13]] > cb)
                   if( p[offsets[14]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[11]] > cb)
                if( p[offsets[12]] > cb)
                 if( p[offsets[13]] > cb)
                  if( p[offsets[14]] > cb)
                   if( p[offsets[15]] > cb)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else if( p[offsets[8]] < c_b)
           if( p[offsets[9]] < c_b)
            if( p[offsets[10]] < c_b)
             if( p[offsets[11]] < c_b)
              if( p[offsets[12]] < c_b)
               if( p[offsets[13]] < c_b)
                if( p[offsets[14]] < c_b)
                 if( p[offsets[15]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[4]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[3]] < c_b)
                if( p[offsets[4]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[offsets[2]] < c_b)
               if( p[offsets[3]] < c_b)
                if( p[offsets[4]] < c_b)
                 if( p[offsets[5]] < c_b)
                  if( p[offsets[6]] < c_b)
                   if( p[offsets[7]] < c_b)
                    goto is_a_corner;
                   else
                    goto is_not_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           goto is_not_a_corner;
        else
         if( p[offsets[7]] > cb)
          if( p[offsets[8]] > cb)
           if( p[offsets[9]] > cb)
            if( p[offsets[6]] > cb)
             if( p[offsets[5]] > cb)
              if( p[offsets[4]] > cb)
               if( p[offsets[3]] > cb)
                if( p[offsets[2]] > cb)
                 if( p[offsets[1]] > cb)
                  goto is_a_corner;
                 else
                  if( p[offsets[10]] > cb)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[10]] > cb)
                  if( p[offsets[11]] > cb)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[10]] > cb)
                 if( p[offsets[11]] > cb)
                  if( p[offsets[12]] > cb)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[10]] > cb)
                if( p[offsets[11]] > cb)
                 if( p[offsets[12]] > cb)
                  if( p[offsets[13]] > cb)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[offsets[10]] > cb)
               if( p[offsets[11]] > cb)
                if( p[offsets[12]] > cb)
                 if( p[offsets[13]] > cb)
                  if( p[offsets[14]] > cb)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             if( p[offsets[10]] > cb)
              if( p[offsets[11]] > cb)
               if( p[offsets[12]] > cb)
                if( p[offsets[13]] > cb)
                 if( p[offsets[14]] > cb)
                  if( p[offsets[15]] > cb)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           goto is_not_a_corner;
         else if( p[offsets[7]] < c_b)
          if( p[offsets[8]] < c_b)
           if( p[offsets[9]] < c_b)
            if( p[offsets[6]] < c_b)
             if( p[offsets[5]] < c_b)
              if( p[offsets[4]] < c_b)
               if( p[offsets[3]] < c_b)
                if( p[offsets[2]] < c_b)
                 if( p[offsets[1]] < c_b)
                  goto is_a_corner;
                 else
                  if( p[offsets[10]] < c_b)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                else
                 if( p[offsets[10]] < c_b)
                  if( p[offsets[11]] < c_b)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
               else
                if( p[offsets[10]] < c_b)
                 if( p[offsets[11]] < c_b)
                  if( p[offsets[12]] < c_b)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
              else
               if( p[offsets[10]] < c_b)
                if( p[offsets[11]] < c_b)
                 if( p[offsets[12]] < c_b)
                  if( p[offsets[13]] < c_b)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
             else
              if( p[offsets[10]] < c_b)
               if( p[offsets[11]] < c_b)
                if( p[offsets[12]] < c_b)
                 if( p[offsets[13]] < c_b)
                  if( p[offsets[14]] < c_b)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
            else
             if( p[offsets[10]] < c_b)
              if( p[offsets[11]] < c_b)
               if( p[offsets[12]] < c_b)
                if( p[offsets[13]] < c_b)
                 if( p[offsets[14]] < c_b)
                  if( p[offsets[15]] < c_b)
                   goto is_a_corner;
                  else
                   goto is_not_a_corner;
                 else
                  goto is_not_a_corner;
                else
                 goto is_not_a_corner;
               else
                goto is_not_a_corner;
              else
               goto is_not_a_corner;
             else
              goto is_not_a_corner;
           else
            goto is_not_a_corner;
          else
           goto is_not_a_corner;
         else
          goto is_not_a_corner;

is_a_corner:
            bmin=b;
            goto end_if;

is_not_a_corner:
            bmax=b;
            goto end_if;

end_if:

        if(bmin == bmax - 1 || bmin == bmax)
        {
            #pragma omp critical
            {
                corner_id = *corners_found;
                *corners_found+=1;
            }
            corners[corner_id*3]   = x;
            corners[corner_id*3+1] = y+Y;
            corners[corner_id*3+2] = bmin;
            return;
        }
        else
            b = (bmin + bmax) / 2;
    }
#endif
}//circular_detection

void FAST_kernel(uint32_t *corners_found,
                 uint32_t *corners,
                 IMG_DATATYPE *input,
                 uint32_t width,
                 uint32_t height,
                 int32_t *offsets,
                 uint32_t Y)
{
    int32_t i;
    int32_t iterations = height*width;

    #pragma omp for schedule(dynamic, 4)
    for(i = 0; i < iterations; i++)
    {
        int32_t x,y;

        x = i % width;
        y = (i / width) + BORDER_FAST9;

        if(x > (int32_t) BORDER_FAST9 && x < (int32_t) (width-BORDER_FAST9))
            circular_detection(x, y, corners_found, corners, input, width, offsets, Y);
    }

    return;
} //FAST_kernel
