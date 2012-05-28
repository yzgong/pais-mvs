#ifndef PAIS_PARTICLE_H
#define PAIS_PARTICLE_H

namespace PAIS {
    class Particle{
    public:
        // parameter dimension
        int dim;
        // best parameter
        double *pBest;
        // current parameter
        double *pos;
        // current velocity
        double *vec;
        // current score
        double score;
        // best score
        double pBestScore;

        Particle();
        Particle(int dim);
        Particle(const Particle &p);
        Particle& operator=(const Particle &p);
        ~Particle(void);
    };
}

#endif