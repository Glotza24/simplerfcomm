#include <unistd.h>

#include "bt.h"

int main(int argc, char *argv[]) {
    Bluetooth bt;
    bt.Enable();


    QCoreApplication app(argc, argv);
    return app.exec();
    
    return 0;
}
