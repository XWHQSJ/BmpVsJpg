#define M_SOF0  0xc0
#define M_DHT   0xc4
#define M_EOI   0xd9
#define M_SOS   0xda
#define M_DQT   0xdb
#define M_DRI   0xdd
#define M_APP0  0xe0

static int Zig_Zag[8][8]={{0,1,5,6,14,15,27,28},
{2,4,7,13,16,26,29,42},
{3,8,12,17,25,30,41,43},
{9,11,18,24,37,40,44,53},
{10,19,23,32,39,45,52,54},
{20,22,33,38,46,51,55,60},
{21,34,37,47,50,56,59,61},
{35,36,48,49,57,58,62,63}
};

#define W1 2841 /* 2048*sqrt(2)*cos(1*pi/16) */
#define W2 2676 /* 2048*sqrt(2)*cos(2*pi/16) */
#define W3 2408 /* 2048*sqrt(2)*cos(3*pi/16) */
#define W5 1609 /* 2048*sqrt(2)*cos(5*pi/16) */
#define W6 1108 /* 2048*sqrt(2)*cos(6*pi/16) */
#define W7 565  /* 2048*sqrt(2)*cos(7*pi/16) */
