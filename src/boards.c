#include "boards.h"
#include "leds.h"

static rev_t board_revision = OLD_REV;

char is_new_rev(void)
{
    return (board_revision == NEW_REV);
}

void set_rev(rev_t rev)
{
    board_revision = rev;
}

