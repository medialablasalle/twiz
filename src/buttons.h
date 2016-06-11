#ifndef BUTTONS_H
#define BUTTONS_H

/**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */
#define BUTTON_DETECTION_DELAY APP_TIMER_TICKS(50, APP_TIMER_PRESCALER)

void gpiote_init(void);
void buttons_init(void);

#endif

