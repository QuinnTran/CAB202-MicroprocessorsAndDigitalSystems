#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

unsigned int read_binary();
void print_binary(unsigned int value, int bits);
void print_result(const char *label, uint8_t value);

int main(void)
{
    puts("Please enter an integer a in base 2: ");
    uint8_t a = read_binary();

    puts("Please enter an integer b in base 2: ");
    uint8_t b = read_binary();

    print_result("     a", a);
    print_result("     b", b);
    print_result("    ~a", ~a);
    print_result("    ~b", ~b);
    print_result(" a | b", a | b);
    print_result(" a & b", a&b);
    print_result(" a ^ b", a^b);
    print_result(" a + b", a + b);
    print_result("    -b", -b);
    print_result("a + -b", a + (-b));
    print_result("a << b", a << b);
    print_result("a >> b", a >> b);
}

unsigned int read_binary()
{
    unsigned int x = 0;
    int ch = fgetc(stdin);

    // Skip over leading spaces.
    while (isspace(ch))
    {
        ch = fgetc(stdin);
    }

    // try to parse an integer
    while (ch == '0' || ch == '1')
    {
        x = x * 2 + (ch - '0');
        ch = fgetc(stdin);
    }

    ungetc(ch, stdin);

    return x;
}

void print_binary(unsigned int value, int bits)
{
    for (int i = bits-1; i >= 0; i--)
    {
        uint8_t mask = 1 << i;
        uint8_t bit_val = (value & mask) >> i;
        fputc(bit_val + '0', stdout);
    }
}

void print_result(const char * label, uint8_t value)
{
    printf("%s = decimal %03d, hexadecimal %02x, or binary ", label, value, value);
    print_binary(value,8);
    printf("\n");
}