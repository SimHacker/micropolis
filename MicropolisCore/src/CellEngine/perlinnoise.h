////////////////////////////////////////////////////////////////////////
/** @file perlinnoise.h Perlin Noise Generator Class
 * Adapted from Intel's optimized C code by Don Hopkins.
 */


#ifndef _PERLIN_NOISE_H_
#define _PERLIN_NOISE_H_ 1


class PerlinNoise {

        enum {
                kDefaultSpeed = 0xffff
        };

private:

        long u;
        long v;
        long du;
        long dv;

        int Random()
        {
                return 
                        (rand() ^ 
                         (rand() << 16));
        }

        int Random1(int i) 
        { 
                return ((i * i) & 0xffff); 
        }

        int Random2(int i)
        {
                return (Random1(i) >> 2); 
        }

        void RandomPosition()
        {
                u = Random();
                v = Random();
        }

        void Normalize(
                long *px,
                long *py,
                long len) 
        {
                float x = (float)*px;
                float y = (float)*py;
                float dist2 =
                        (x * x) + (y * y);
                if (dist2 > (float)0.0) {
                        float dist = 
                                (float)sqrt((float)dist2);
                        float scale = 
                                len / dist;
                        *px = (int)floor((float)0.5 + (x * scale));
                        *py = (int)floor((float)0.5 + (y * scale));
                } // if
        }

public:

        void SetState(
                long u0,
                long v0,
                long du0,
                long dv0) 
        {
                u = u0;
                v = v0;
                du = du0;
                dv = dv0;
        }

        void GetState(
                long *pu,
                long *pv,
                long *pdu,
                long *pdv) 
        {
                *pu = u;
                *pv = v;
                *pdu = du;
                *pdv = dv;
        }

        void SetSpeed(long speed) 
        {
                du = Random() >> 8;
                dv = Random() >> 8;

                Normalize(&du, &dv, speed);
        }

        PerlinNoise() 
        {
                RandomPosition();
                SetSpeed(kDefaultSpeed);
        }

        PerlinNoise(long speed) 
        {
                RandomPosition();
                SetSpeed(speed);
        }

        PerlinNoise(
                long du0,
                long dv0) 
        {
                RandomPosition();
                du = dv0;
                dv = du0;
        }

        PerlinNoise(
                long u0,
                long v0,
                long du0,
                long dv0) 
        {
                SetState(u0, v0, du0, dv0);
        }

        void Generate(
                unsigned char *data,
                int count = 1)
        {
                // Inner loop for the scan line.  "count" pixels will be drawn.
                unsigned short i;
                for (i = 0; i < count; i++) {

                        // Convert the u and v parameters from 10.22 to 8.8 format.
                        unsigned short u_16bit =
                                (short)(u >> 14);
                        unsigned short v_16bit =
                                (short)(v >> 14);

                        // Imagine having a square of the type
                        //  p0---p1    Where p0 = (bx0, by0)   +----> U
                        //  |(u,v)|          p1 = (bx1, by0)   |
                        //  |     |          p2 = (bx0, by1)   |    Coordinate System
                        //  p2---p3          p3 = (bx1, by1)   V
                        // The u, v point in 2D texture space is bounded by this rectangle.

                        // Goal: determine the color of the points p0, p1, p2, p3.  
                        // Then the color of the point (u, v) will be found by linear interpolation.

                        // First step:  Get the 2D coordinates of the points p0, p1, p2, p3.

                        // Same as Perlin's original code except floating point 
                        // is converted over to fixed integer.

                        // Process the x component.
                        unsigned char bx0 =    // Integer part of u, x component of p0 and p2.
                                u_16bit >> 8;
                        unsigned char bx1 =    // Integer part of u + 1, x component of p1 and p3.
                                bx0 + 1;

                        // Process the y component.
                        unsigned char by0 =    // Integer part of v, y component of p0 and p1.
                                v_16bit >> 8;
                        unsigned char by1 =    // Integer part of v + 1, y component of p2 and p3.
                                by0 + 1;

                        // Next, we need vectors pointing from each point in the square above and
                        // ending at the (u,v) coordinate located inside the square.
                        // The vector (rx0, ry0) goes from P0 to the (u,v) coordinate.
                        // The vector (rx1, ry0) goes from P1 to the (u,v) coordinate.
                        // The vector (rx0, ry1) goes from P2 to the (u,v) coordinate.
                        // The vector (rx1, ry1) goes from P3 to the (u,v) coordinate.

                        // Process the x component.
                        unsigned char rx0 =   // Fractional part of u. (0.0 <= rx0 < 1.0) or (0 <= rx0 < 255) for fixed math.
                                u_16bit & 255;
                        signed short rx1 =   // Fractional part of u + 1. (-1.0 < rx1 <= 0.0) or (-255 < rx1 <= 0) for fixed math.
                                rx0 - 256;

                        // Process the y component.
                        unsigned char ry0 =   // Fractional part of v. (0.0 <= ry0 < 1.0) or (0 <= ry0 < 255) for fixed math.
                                v_16bit & 255;
                        signed short ry1 =   // Fractional part of v + 1. (-1.0 < rx1 <= 0.0) or (-255 < rx1 <= 0) for fixed math.
                                ry0 - 256;

                        // Next, calculate the dropoff component about the point p0.

                        // Same as Perlin's original code except floating point
                        // is converted over to fixed integer.  The ">> 1" is used to
                        // avoid overflow when the values are multiplied together.
                        // This is not a problem in "C" but will be in the MMX implementation.

                        signed short sx =
                                (((rx0 * rx0) >> 1) * ((1536 - (rx0 << 2)))) >> 16;
                        signed short sy =
                                (((ry0 * ry0) >> 1) * ((1536 - (ry0 << 2)))) >> 16;

                        // Now, for each point p0, p1, p2, p3 in the square above, imagine having
                        // a unit gradiant vector pointing in any random direction.  This step 
                        // attaches a unit gradiant vector to each point of the square.  This is
                        // done by precalculating 256, random, uniform, unit vectors.  Then to attach
                        // the gradiant vector to a point, an index into the array is needed.  The
                        // index is acquired from the x and y coordinates of the square corner point.
                        // The algorithm used is called "Folding Over".
                        // b00, b10, b01, and b11 contain indexes for a gradiant vector for each
                        // corner of the square shown above.

                        // This is where the code differs from Perlins.
                        // Rather than use arrays p[] and g[][], the values are
                        // calculated real-time.  Here Random1() replaces array p[].
                        // Perlin's equivalent: b00 = p[p[bx0] + by0];

                        // Used to replace array: b00 = p[p[bx0] + by0];

                        short b00 =
                                Random1((Random1(bx0) + by0));
                        short b01 =
                                Random1((Random1(bx0) + by1));
                        short b10 =
                                Random1((Random1(bx1) + by0));
                        short b11 =
                                Random1((Random1(bx1) + by1));

                        // Original Perlin's noise program used an array g[512][2].
                        // This program replaces the array with real-time calculations.
                        // The results are stored into variables of the form g_b##_#.

                        // Here, Random2() replaces array g[][].
                        // Perlin's equivalent: g_b00_0 = g[b00][0];

                        short g_b00_0 =       // Used to replace array g[b00][0]
                                (Random2(b00) & 511) - 256;
                        short g_b01_0 =       // replaces array g[b01][0]
                                (Random2(b01) & 511) - 256;
                        short g_b10_0 =       // replaces array g[b10][0]
                                (Random2(b10) & 511) - 256;
                        short g_b11_0 =       // replaces array g[b11][0]
                                (Random2(b11) & 511) - 256;

                        short g_b00_1 =       // Used to replace array g[b00][1]
                                (Random2((b00 + 1)) & 511) - 256;
                        short g_b01_1 =       // replaces array g[b01][1]
                                (Random2((b01 + 1)) & 511) - 256;
                        short g_b10_1 =       // replaces array g[b10][1]
                                (Random2((b10 + 1)) & 511) - 256;
                        short g_b11_1 =       // replaces array g[b11][1]
                                (Random2((b11 + 1)) & 511) - 256;

                        // Now, for each point in the square shown above, calculate the dot
                        // product of the gradiant vector and the vector going from each square
                        // corner point to the (u,v) point inside the square.

                        // Same as Perlin's original code.

                        signed long color_p0 =
                                (rx0 * g_b00_0) + 
                                (ry0 * g_b00_1);
                        signed long color_p1 =
                                (rx1 * g_b10_0) + 
                                (ry0 * g_b10_1);
                        signed long color_p2 =
                                (rx0 * g_b01_0) + 
                                (ry1 * g_b01_1);
                        signed long color_p3 =
                                (rx1 * g_b11_0) + 
                                (ry1 * g_b11_1);

                        // color_p0, color_p1, ... are the colors of the points p0, p1, p2, p3.
                        // Now use linear interpolation to get the color of the point (sx, sy) inside
                        // the square.

                        // Interpolation along the X axis.

                        signed long color_y0 =
                                color_p0 + sx * ((color_p1 - color_p0) >> 8);
                        signed long color_y1 =
                                color_p2 + sx * ((color_p3 - color_p2) >> 8);

                        // Interpolation along the Y axis.
                        // Output is from -256 to +256, so a 256 color offset is added and
                        // the final result is divided by 2.  (>>1).  (0 <= next_color < 256).

                        // Same as Perlin's original code except the output is 
                        // converted from fixed point to regular integer.  Also
                        // since the output ranges from -256 to +256, a 256 offest
                        // is added to make the range from 0 to 511.  This offset
                        // is the 65536 value.  Then the 0 to 511 is scaled down.
                        // to a range of 0 to 255.

                        unsigned char color =
                                (unsigned char)
                                        ((color_y0 + 
                                          65536 + 
                                          (sy * ((color_y1 - color_y0) >> 8))
                                         ) >> 9);

                        data[i] = color;

                        u += du;  // New u for the next pixel
                        v += dv;  // New v for the next pixel
                } // for x
        }
};
 
class QuatNoise {
         int speed;
         PerlinNoise xNoise;
         PerlinNoise yNoise;
         PerlinNoise zNoise;
         PerlinNoise wNoise;

public:
        QuatNoise() {}

         QuatNoise(int speed0) :
                speed(speed0),
                xNoise(speed0),
                yNoise(speed0),
                zNoise(speed0),
                wNoise(speed0) {}

        int GetSpeed()
        {
                return speed;
        }

        void SetSpeed(int s)
        {
                if (speed != s) {
                        speed = s;
                        xNoise.SetSpeed(s);
                        yNoise.SetSpeed(s);
                        zNoise.SetSpeed(s);
                        wNoise.SetSpeed(s);
                } // if
        }

        void Blend(
                float *quat,
                float magnitude)
        {
                unsigned char nx, ny, nz, nw;

                xNoise.Generate(&nx);
                yNoise.Generate(&ny);
                zNoise.Generate(&nz);
                wNoise.Generate(&nw);

                float mag =
                        magnitude / (float)128.0;

                float dx =
                        (float)((short)nx - 128) * mag;
                float dy =
                        (float)((short)ny - 128) * mag;
                float dz =
                        (float)((short)nz - 128) * mag;
                float dw =
                        (float)((short)nw - 128) * mag;

                float x =
                        quat[0] + dx;
                float y =
                        quat[1] + dy;
                float z =
                        quat[2] + dz;
                float w =
                        quat[3] + dw;

                float dist2 =
                        (x * x) + 
                        (y * y) + 
                        (z * z) + 
                        (w * w);

                float dist =
                        (float)sqrt(dist2);
                float scale =
                        (float)1.0 / dist;

                quat[0] = x * scale;
                quat[1] = y * scale;
                quat[2] = z * scale;
                quat[3] = w * scale;
        }
};


#endif _PERLIN_NOISE_H_
