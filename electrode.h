#ifndef ELECTRODE_H
#define ELECTRODE_H

#define NO_CONNECTION 0
#define OKAY_CONNECTION 1
#define GOOD_CONNECTION 2

class Electrode
{
public:
    Electrode();
    int getConnection();
    void setConnection(int);

private:
    int connection = 0;
};

#endif // ELECTRODE_H
