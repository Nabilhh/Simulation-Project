#include "electrode.h"

Electrode::Electrode()
{

}

//setter of the connection of the earclip
void Electrode::setConnection(int status){
    if(status < 0){
        connection = NO_CONNECTION;
    } else if(status > 2){
        connection = GOOD_CONNECTION;
    } else {
        connection = status;
    }
}

int Electrode::getConnection(){
    return connection;
}
