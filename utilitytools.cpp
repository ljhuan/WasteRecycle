#include "utilitytools.h"

UtilityTools::UtilityTools()
{

}

QString UtilityTools::holdPlaces(int num)
{
    QString places = "";
    for (int i = 0; i < num; ++i) {
        places += " ";
    }
    return places;
}
