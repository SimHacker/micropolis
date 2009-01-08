/* animate.cpp
 * Animate Tiles, for Unix Micropolis.
 *
 * Micropolis, Unix Version.  This game was released for the Unix platform
 * in or about 1990 and has been modified for inclusion in the One Laptop
 * Per Child program.  Copyright (C) 1989 - 2007 Electronic Arts Inc.  If
 * you need assistance with this program, you may contact:
 *   http://wiki.laptop.org/go/Micropolis  or email  micropolis@laptop.org.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 *
 *             ADDITIONAL TERMS per GNU GPL Section 7
 *
 * No trademark or publicity rights are granted.  This license does NOT
 * give you any right, title or interest in the trademark SimCity or any
 * other Electronic Arts trademark.  You may not distribute any
 * modification of this program using the trademark SimCity or claim any
 * affliation or association with Electronic Arts Inc. or its employees.
 *
 * Any propagation or conveyance of this program must include this
 * copyright notice and these terms.
 *
 * If you convey this program (or any modifications of it) and assume
 * contractual liability for the program to recipients of it, you agree
 * to indemnify Electronic Arts for any liability that those contractual
 * assumptions impose on Electronic Arts.
 *
 * You may not misrepresent the origins of this program; modified
 * versions of the program must be marked as such and not identified as
 * the original program.
 *
 * This disclaimer supplements the one included in the General Public
 * License.  TO THE FULLEST EXTENT PERMISSIBLE UNDER APPLICABLE LAW, THIS
 * PROGRAM IS PROVIDED TO YOU "AS IS," WITH ALL FAULTS, WITHOUT WARRANTY
 * OF ANY KIND, AND YOUR USE IS AT YOUR SOLE RISK.  THE ENTIRE RISK OF
 * SATISFACTORY QUALITY AND PERFORMANCE RESIDES WITH YOU.  ELECTRONIC ARTS
 * DISCLAIMS ANY AND ALL EXPRESS, IMPLIED OR STATUTORY WARRANTIES,
 * INCLUDING IMPLIED WARRANTIES OF MERCHANTABILITY, SATISFACTORY QUALITY,
 * FITNESS FOR A PARTICULAR PURPOSE, NONINFRINGEMENT OF THIRD PARTY
 * RIGHTS, AND WARRANTIES (IF ANY) ARISING FROM A COURSE OF DEALING,
 * USAGE, OR TRADE PRACTICE.  ELECTRONIC ARTS DOES NOT WARRANT AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE PROGRAM; THAT THE PROGRAM WILL
 * MEET YOUR REQUIREMENTS; THAT OPERATION OF THE PROGRAM WILL BE
 * UNINTERRUPTED OR ERROR-FREE, OR THAT THE PROGRAM WILL BE COMPATIBLE
 * WITH THIRD PARTY SOFTWARE OR THAT ANY ERRORS IN THE PROGRAM WILL BE
 * CORRECTED.  NO ORAL OR WRITTEN ADVICE PROVIDED BY ELECTRONIC ARTS OR
 * ANY AUTHORIZED REPRESENTATIVE SHALL CREATE A WARRANTY.  SOME
 * JURISDICTIONS DO NOT ALLOW THE EXCLUSION OF OR LIMITATIONS ON IMPLIED
 * WARRANTIES OR THE LIMITATIONS ON THE APPLICABLE STATUTORY RIGHTS OF A
 * CONSUMER, SO SOME OR ALL OF THE ABOVE EXCLUSIONS AND LIMITATIONS MAY
 * NOT APPLY TO YOU.
 */

/** @file animate.cpp
 * Animate Tiles, for Unix Micropolis.
 *
 * @todo Generate the table below from, the animation sequences file
 *       doc/AnimationSequences.txt and the doc/genAnimationTable.py
 *       program
 */

////////////////////////////////////////////////////////////////////////


#include "stdafx.h"


////////////////////////////////////////////////////////////////////////


static short aniTile[1024] = {
    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,
    /* Fire */
                                            57,  58,  59,  60,  61,  62,  63,  56,
    /* No Traffic */
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    /* Light Traffic */
    128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
    96,  97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
    112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    /* Heavy Traffic */
    192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
    144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
    160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
    176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
    /* Wires & Rails */
    208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
    224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
    /* Residential */
    240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
    256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271,
    272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287,
    288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303,
    304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319,
    320, 321, 322, 323, 324, 325, 326, 327, 328, 329, 330, 331, 332, 333, 334, 335,
    336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351,
    352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367,
    368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383,
    384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399,
    400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415,
    416, 417, 418, 419, 420, 421, 422,
    /* Commercial */
                                       423, 424, 425, 426, 427, 428, 429, 430, 431,
    432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447,
    448, 449, 450, 451, 452, 453, 454, 455, 456, 457, 458, 459, 460, 461, 462, 463,
    464, 465, 466, 467, 468, 469, 470, 471, 472, 473, 474, 475, 476, 477, 478, 479,
    480, 481, 482, 483, 484, 485, 486, 487, 488, 489, 490, 491, 492, 493, 494, 495,
    496, 497, 498, 499, 500, 501, 502, 503, 504, 505, 506, 507, 508, 509, 510, 511,
    512, 513, 514, 515, 516, 517, 518, 519, 520, 521, 522, 523, 524, 525, 526, 527,
    528, 529, 530, 531, 532, 533, 534, 535, 536, 537, 538, 539, 540, 541, 542, 543,
    544, 545, 546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559,
    560, 561, 562, 563, 564, 565, 566, 567, 568, 569, 570, 571, 572, 573, 574, 575,
    576, 577, 578, 579, 580, 581, 582, 583, 584, 585, 586, 587, 588, 589, 590, 591,
    592, 593, 594, 595, 596, 597, 598, 599, 600, 601, 602, 603, 604, 605, 606, 607,
    608, 609, 610, 611,
    /* Industrial */
                        612, 613, 614, 615, 616, 617, 618, 619, 852, 621, 622, 623,
    624, 625, 626, 627, 628, 629, 630, 631, 632, 633, 634, 635, 636, 637, 638, 639,
    640, 884, 642, 643, 888, 645, 646, 647, 648, 892, 896, 651, 652, 653, 654, 655,
    656, 657, 658, 659, 660, 661, 662, 663, 664, 665, 666, 667, 668, 669, 670, 671,
    672, 673, 674, 675, 900, 904, 678, 679, 680, 681, 682, 683, 684, 685, 908, 687,
    688, 912, 690, 691, 692,
    /* SeaPort */
                             693, 694, 695, 696, 697, 698, 699, 700, 701, 702, 703,
    704, 705, 706, 707, 708,
    /* AirPort */
                             709, 710, 832, 712, 713, 714, 715, 716, 717, 718, 719,
    720, 721, 722, 723, 724, 725, 726, 727, 728, 729, 730, 731, 732, 733, 734, 735,
    736, 737, 738, 739, 740, 741, 742, 743, 744,
    /* Coal power */
                                                 745, 746, 916, 920, 749, 750, 924,
    928, 753, 754, 755, 756, 757, 758, 759, 760,
    /* Fire Dept */
                                                 761, 762, 763, 764, 765, 766, 767,
    768, 769, 770, 771, 772, 773, 774, 775, 776, 777, 778,
    /* Stadium */
                                                           779, 780, 781, 782, 783,
    784, 785, 786, 787, 788, 789, 790, 791, 792, 793, 794,
    /* Stadium Anims */
                                                           795, 796, 797, 798, 799,
    800, 801, 802, 803, 804, 805, 806, 807, 808, 809, 810,
    /* Nuclear Power */
                                                           811, 812, 813, 814, 815,
    816, 817, 818, 819, 952, 821, 822, 823, 824, 825, 826,
    /* Power out + Bridges */
                                                           827, 828, 829, 830, 831,
    /* Radar dish */
    833, 834, 835, 836, 837, 838, 839, 832,
    /* Fountain / Flag */
                                            841, 842, 843, 840, 845, 846, 847, 848,
    849, 850, 851, 844, 853, 854, 855, 856, 857, 858, 859, 852,
    /* zone destruct & rubblize */
                                                                861, 862, 863, 864,
    865, 866, 867, 867,
    /* totally unsure */
                        868, 869, 870, 871, 872, 873, 874, 875, 876, 877, 878, 879,
    880, 881, 882, 883,
    /* Smoke stacks */
                        885, 886, 887, 884, 889, 890, 891, 888, 893, 894, 895, 892,
    897, 898, 899, 896, 901, 902, 903, 900, 905, 906, 907, 904, 909, 910, 911, 908,
    913, 914, 915, 912, 917, 918, 919, 916, 921, 922, 923, 920, 925, 926, 927, 924,
    929, 930, 931, 928,
    /* Stadium Playfield */
                        933, 934, 935, 936, 937, 938, 939, 932, 941, 942, 943, 944,
    945, 946, 947, 940,
    /* Bridge up chars */
                        948, 949, 950, 951,
    /* Nuclear swirl */
                                            953, 954, 955, 952,
    /*  */
};


////////////////////////////////////////////////////////////////////////


/* comefrom: moveWorld doEditWindow scoreDoer doMapInFront graphDoer */
void Micropolis::animateTiles()
{
    unsigned short tilevalue, tileflags;
    unsigned short *tMapPtr;
    int i;

    /* Animate whole world */
    tMapPtr = (unsigned short *)&(Map[0][0]);

    for (i = WORLD_X * WORLD_Y; i > 0; i--) {
	tilevalue = (*tMapPtr);
	if (tilevalue & ANIMBIT) {
	    tileflags = tilevalue & ALLBITS;
	    tilevalue &= LOMASK;
	    tilevalue = aniTile[tilevalue];
	    tilevalue |= tileflags;
	    (*tMapPtr) = tilevalue;
	}
	tMapPtr++;
    }
}


////////////////////////////////////////////////////////////////////////
