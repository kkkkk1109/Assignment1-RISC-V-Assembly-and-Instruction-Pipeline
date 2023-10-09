#include <stdint.h>
#include <stdio.h>
#define iteration 5
int lz, lzc, i;
float ans;
int count_leading_zeros(uint32_t x)
{
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    /* count ones (population count) */
    x -= ((x >> 1) & 0x55555555);
    x = ((x >> 2) & 0x33333333) + (x & 0x33333333);
    x = ((x >> 4) + x) & 0x0f0f0f0f;
    x += (x >> 8);       
    x += (x >> 16);
    
    return (32 - (x & 0x7f));
}
float uint_to_float(uint32_t u){
    int c = count_leading_zeros(u);
    unsigned int exp=127+31-c;
    u <<= (c-8);
    uint32_t flt= (exp << 23) | (u & 0x7FFFFF);
    return * (float *) &flt;
}
float division(float a,float b){
    //change float into int 
    int32_t ia = *(int32_t *)& a;
    int32_t ib = *(int32_t *)& b;
    //get the exponential
    int32_t expa = (ia >> 23) & 0xff ;
    int32_t expb = (ib >> 23) & 0xff;
    // parameter used in rounded output
    uint8_t  odd, rnd, sticky;
    //get the significand
    uint32_t siga= ((ia  & 0x7fffff) | 0x800000);
    uint32_t sigb= ((ib  & 0x7fffff) | 0x800000);
    //exponential output
    int32_t expout= expa - expb +127;
    //allign two numbers' significand
    if(siga < sigb){
        siga = siga << 1;
        expout--;
    }
    // r means result
    uint32_t r = 0;
    // star division
    for(int i= 0 ; i < 25 ; i++){
        r= r << 1;
        if(siga >= sigb){
            siga = siga - sigb;
            r = r | 1;
        }
        siga = siga << 1 ;
    }
    // rounded
    sticky =(siga != 0);
    rnd =(r & 1);
    odd =(r & 2) != 0;
    r=(r>>1)+ (rnd & (sticky | odd));
    int32_t sigout=r & 0x7fffff;
    int32_t out= ((expout & 0xff) << 23) | (sigout);
    return *(float *) &out;
}
float addition(float a,float b){
    //change float into int
    int32_t ia = *(int32_t *)& a;
    int32_t ib = *(int32_t *)& b;
    //get the exponential
    int32_t expa = (ia >> 23) & 0xff ;
    int32_t expb = (ib >> 23) & 0xff;
    //get the significand
    uint32_t siga= ((ia & 0x7fffff) | 0x800000);
    uint32_t sigb= ((ib & 0x7fffff) | 0x800000);
    //exponential output
    int32_t expout=0;
    int dif= expa - expb;
    if(dif >= 0){
        sigb = sigb >> dif;
        expout = expa;
    }
    else{
        dif = -dif;
        siga = siga >> dif;
        expout = expb;
    }
    uint32_t sigout = siga + sigb;
    if(sigout >> 24 == 1){
        sigout = sigout >>1;
        expout = expout + 1;
    }
    int32_t out=(0 & 1 << 31) | ((expout & 0xff) << 23) | ((sigout) & 0x7fffff);
    return *(float *) &out;
}
int main(){
    uint32_t r=500;
    lz =count_leading_zeros(r);
    lzc = (32 - lz) / 2;
    ans=uint_to_float(r >> lzc);
    float t,c;
    for (i = 0 ; i < iteration ; i++){
        t = division(r,ans);
        t = addition(t,ans);
        t = division(t,2);
        ans=t;
        if(ans == c){
            break;
        }
        c = ans;
    }
    printf("After %d iterations, the square root of %d is %f",i+1,r,ans);
    return 0; 
}
