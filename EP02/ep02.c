/***************************
** 
** NOME: Guilherme M Bortoletto
** MATR: 2019007734
** DISC: SIN120
** EXER: EP02
** DATA: 15.09.2020
** 
****************************/

#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define PI 3.14159265

int m_is_precise(double value)
{
    if (value < 0) value *= -1;
    if (value == 0) return 0;
    return (int) (value < 0.00000001) ? 1 : 0;
}

int m_fact(int number)
{
    if (number != 0)
    {
        int factorial = 1;
        for (int i = number; i > 0; --i)
        {
            factorial *= i;
        }
        return factorial;
    }
    return 1;
}

double m_sin(double x)
{
    double sin_x = 0;
    double current = 0;
    int i = 1;
    int j = 0;
    sin_x += pow(x, i) / m_fact(i);
    while (!m_is_precise(current))
    {
        i += 2;
        current = pow(x, i) / m_fact(i);
        if (j++ % 2 == 0) current *= -1;
        sin_x += current;
    }
    return sin_x;
}

double m_cos(double x)
{
    double cos_x = 0;
    double current = 0;
    int i = 0;
    int j = 0;
    cos_x += pow(x, i) / m_fact(i);
    while (!m_is_precise(current))
    {
        i += 2;
        current = pow(x, i) / m_fact(i);
        if (j++ % 2 == 0) current *= -1;
        cos_x += current;
    }
    return cos_x;
}

void non_linear(double _angle)
{
    clock_t begin, end;
    double elapsed;
    begin = clock();
    int p_sin = 0;
    int p_cos = 0;
    int fd_sin_1[2];
    int fd_cos_1[2];
    int fd_sin_2[2];
    int fd_cos_2[2];
    if (!(pipe(fd_sin_1) || pipe(fd_sin_2)) && (p_sin = fork() > 0))
    {
        if (!(pipe(fd_cos_1) || pipe(fd_cos_2)) && (p_cos = fork() > 0))
        {
            close(fd_sin_1[0]);
            write(fd_sin_1[1], &_angle, sizeof _angle);
            close(fd_sin_1[1]);
            close(fd_cos_1[0]);
            write(fd_cos_1[1], &_angle, sizeof _angle);
            close(fd_cos_1[1]);
            double sin_x;
            close(fd_sin_2[1]);
            read (fd_sin_2[0], &sin_x, sizeof sin_x);
            close(fd_sin_2[0]);
            double cos_x;
            close(fd_cos_2[1]);
            read (fd_cos_2[0], &cos_x, sizeof cos_x);
            close(fd_cos_2[0]);
            double tan_x = sin_x / cos_x;
            end = clock();
            elapsed = (double) (end - begin) / CLOCKS_PER_SEC;
            printf("NON-LINEAR: %.8lf segundos\n", elapsed);
        }
        else
        {
            double angle;
            close(fd_cos_1[1]);
            read (fd_cos_1[0], &angle, sizeof angle);
            close(fd_cos_1[0]);
            double cos_x = m_cos(angle);
            close(fd_cos_2[0]);
            write(fd_cos_2[1], &cos_x, sizeof cos_x);
            close(fd_cos_2[1]);
        }
    }
    else
    {
        double angle;
        close(fd_sin_1[1]);
        read (fd_sin_1[0], &angle, sizeof angle);
        close(fd_sin_1[0]);
        double sin_x = m_sin(angle);
        close(fd_sin_2[0]);
        write(fd_sin_2[1], &sin_x, sizeof sin_x);
        close(fd_sin_2[1]);
    }
}

void linear(double angle)
{
    clock_t begin, end;
    double elapsed = 0;
    begin = clock();
    double sin_x = m_sin(angle);
    double cos_x = m_cos(angle);
    double tan_x = sin_x / cos_x;
    end = clock();
    elapsed = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("    LINEAR: %.8lf segundos\n", elapsed);
}

int main(int argc, char* argv[])
{
    double angle = PI / 6;
    linear(angle);
    non_linear(angle);
    return 0;
}
