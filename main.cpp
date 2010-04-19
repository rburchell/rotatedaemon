#include "rotationdaemon.h"

int main(int argc, char **argv)
{
    RotationDaemon a(argc, argv);

    a.exec();
}
