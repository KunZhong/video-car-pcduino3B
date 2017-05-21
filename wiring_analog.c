#include "wiring_analog.h"

static const char *pwm_dev = "/dev/pwmtimer";

typedef struct tagPWM_Config {
    int channel;
    int dutycycle;
} PWM_Config,*pPWM_Config;

typedef struct tagPWM_Freq {
    int channel;
    int step;
    int pre_scale;
    unsigned int freq;
} PWM_Freq,*pPWM_Freq;

#define PWMTMR_START    (0x101)
#define PWMTMR_STOP     (0x102)
#define PWMTMR_FUNC     (0x103)
#define PWMTMR_TONE     (0x104)
#define PWM_CONFIG      (0x105)
#define HWPWM_DUTY      (0x106)
#define PWM_FREQ        (0x107)

#define MAX_PWMTMR_FREQ (2000)   /* 2kHz pin 3,9,10,11 */
#define MIN_PWMTMR_FREQ (126)    /* 126Hz pin 3,9,10,11 */
#define MAX_PWMHW_FREQ  (20000)  /* 20kHz pin 5,6 */

/* under construct */
uint8_t analog_reference = 0;
void analogReference(uint8_t mode)
{
    analog_reference = mode;
}

/*  
 * pin(3/9/10/11) support frequency[125-2000]Hz @different dutycycle
 * pin(5/6) support frequency[195,260,390,520,781] @256 dutycycle
 */
int pwmfreq_set(uint8_t pin, unsigned int freq)
{
     int ret = -1;
     int fd = -1;
     PWM_Freq pwmfreq;
         
     if ( (pin == 3 || pin == 5 || pin == 6 || pin == 9 || pin == 10 || pin == 11) && (freq > 0) )
     {
         pwmfreq.channel = pin;
         pwmfreq.freq = freq;
         pwmfreq.step = 0;
         fd = open(pwm_dev, O_RDONLY);
         if ( fd < 0 )
             pabort("open pwm device fail");

         switch (pin) 
         {
         case 5:
         case 6:
            if ( (freq == 195) || (freq == 260) || (freq == 390) 
                || (freq == 520) || (freq == 781) )
            {
               ret = ioctl(fd, PWM_FREQ, &pwmfreq);
               if (ret < 0)
                  pabort("can't set PWM_FREQ");
            }else
               fprintf(stderr, "%s ERROR: invalid frequency, should be [195,260,390,520,781], pin=%d\n", __FUNCTION__, pin);

            break;
         case 3:
         case 9:    
         case 10:   
         case 11:   
            if ((freq >= MIN_PWMTMR_FREQ) && (freq <= MAX_PWMTMR_FREQ)){
               pin = 
               ret = ioctl(fd, PWMTMR_STOP, &pwmfreq.channel);
               if (ret < 0)
                  pabort("can't set PWMTMR_STOP");  
                          
               ret = ioctl(fd, PWM_FREQ, &pwmfreq);
               if (ret < 0)
                  pabort("can't set PWM_FREQ");   
            }else
               fprintf(stderr, "%s ERROR: invalid frequency[%d,%d], pin=%d\n", __FUNCTION__, MIN_PWMTMR_FREQ, MAX_PWMTMR_FREQ, pin);

            break;

         default:   
            break;
         }

         if(fd)
             close(fd);
         return pwmfreq.step;
     }
     else
     {
         fprintf(stderr, "%s ERROR: invalid pin, pin=%d\n", __FUNCTION__, pin);
         exit(-1);
     } 
     return 0;
}

void analogWrite(uint8_t pin, int value)
{
     int ret = -1;
     int fd = -1;
     int val = 0;
     PWM_Config pwmconfig;
         
     pwmconfig.channel = pin;
     pwmconfig.dutycycle = value;
     if ( (pin == 3 || pin == 5 || pin == 6 || pin == 9 || pin == 10 || pin == 11) && 
         (value >= 0 && value <= MAX_PWM_LEVEL) )
     {
         fd = open(pwm_dev, O_RDONLY);
         if ( fd < 0 )
             pabort("open pwm device fail");
                
         switch (pin) 
         {
         case 5:
         case 6:
            ret = ioctl(fd, HWPWM_DUTY, &pwmconfig);
            if (ret < 0)
               pabort("can't set HWPWM_DUTY");
            break;
         case 3:
         case 9:    
         case 10:   
         case 11: 
/*  
            //ret = ioctl(fd, PWMTMR_STOP, &pwmconfig.channel);
            //if (ret < 0)
            //   pabort("can't set PWMTMR_STOP");  
*/                      
            ret = ioctl(fd, PWM_CONFIG, &pwmconfig);
            if (ret < 0)
               pabort("can't set PWM_CONFIG");   

            ret = ioctl(fd, PWMTMR_START, &val);
            if (ret < 0)
               pabort("can't set PWMTMR_START");
            break;

         default:   
            break;
         }
         if(fd)
            close(fd);
     }
     else
     {
         fprintf(stderr, "%s ERROR: invalid pin, pin=%d\n", __FUNCTION__, pin);
         exit(-1);
     }      
}

