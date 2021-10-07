#include <iostream>
#include <fstream>
#include <random>
#include <iomanip>

#define ROUND 100
#define GENERATION 100
#define POPULATION 100
#define DIMENSION 10
#define MAX_POSITION 10
#define MIN_POSITION 0
#define MAX_VELOCITY 10
#define MIN_VELOCITY -10
#define CAPACITY 275
#define w 1
#define c1 1
#define c2 1

using namespace std;

int position[POPULATION][DIMENSION];
int pBestPosition[POPULATION][DIMENSION];
int gBestPosition[POPULATION][DIMENSION];
int lastVelocity[POPULATION][DIMENSION];
int velocity[POPULATION][DIMENSION];
int posWeight[POPULATION];
int fitness[POPULATION];
int pBestFitness[POPULATION];
int gBestFitness;

const int weight[DIMENSION] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
const int value[DIMENSION] = {6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

int random(int start, int end) {
    random_device rd;
    default_random_engine gen = default_random_engine(rd());
    uniform_int_distribution<int> dis(start, end);
    return dis(gen);
}

void initialize() {
    gBestFitness = INT_MIN;
    for (int i = 0; i < POPULATION; i++) {
        posWeight[i] = 0;
        fitness[i] = 0;
        pBestFitness[i] = INT_MIN;
        for (int j = 0; j < DIMENSION; j++) {
            pBestPosition[i][j] = 0;
            gBestPosition[i][j] = 0;
            position[i][j] = random(MIN_POSITION, MAX_POSITION);
            lastVelocity[i][j] = random(MIN_VELOCITY, MAX_VELOCITY);
            velocity[i][j] = random(MIN_VELOCITY, MAX_VELOCITY);
        }
    }
}

void dropItem(int i) {
    do {
        for (int j = DIMENSION - 1; j >= 0;) {
            if (position[i][j] != 0) {
                position[i][j] -= 1;
                posWeight[i] -= weight[j];
                fitness[i] -= value[j];
                break;
            } else j--;
        }
    } while (posWeight[i] > CAPACITY);
}

void calcFitness() {
    for (int i = 0; i < POPULATION; i++) {
        fitness[i] = 0;
        posWeight[i] = 0;
        for (int j = 0; j < DIMENSION; j++) {
            posWeight[i] += weight[j] * position[i][j];
            fitness[i] += value[j] * position[i][j];
        }
        if (posWeight[i] > CAPACITY) {
            dropItem(i);
        }
    }
}

void update() {
    for (int i = 0; i < POPULATION; i++) {
        // fitness > pBestFitness
        if (fitness[i] > pBestFitness[i]) {
            pBestFitness[i] = fitness[i];
            for (int j = 0; j < DIMENSION; j++) {
                pBestPosition[i][j] = position[i][j];
            }
        }

        // pBestFitness > gBestFitness
        if (pBestFitness[i] > gBestFitness) {
            gBestFitness = pBestFitness[i];
            for (int j = 0; j < DIMENSION; j++) {
                gBestPosition[i][j] = pBestPosition[i][j];
            }
        }

        // updateVelocity
        for (int j = 0; j < DIMENSION; j++) {
            velocity[i][j] = w * lastVelocity[i][j] +
                             c1 * random(0, 1) * (pBestPosition[i][j] - position[i][j]) +
                             c2 * random(0, 1) * (gBestPosition[i][j] - position[i][j]);

            if (velocity[i][j] > MAX_VELOCITY) {
                velocity[i][j] = MAX_VELOCITY;
            }

            if (velocity[i][j] < MIN_VELOCITY) {
                velocity[i][j] = MIN_VELOCITY;
            }
        }
    }
}

void move() {
    for (int i = 0; i < POPULATION; i++) {
        for (int j = 0; j < DIMENSION; j++) {
            // lastVelocity = velocity
            lastVelocity[i][j] = velocity[i][j];

            // position = position + velocity
            position[i][j] = position[i][j] + velocity[i][j];

            if (position[i][j] > MAX_POSITION) {
                position[i][j] = MAX_POSITION;
            }

            if (position[i][j] < MIN_POSITION) {
                position[i][j] = MIN_POSITION;
            }
        }
    }
}

int main() {
    int count_620 = 0, count_615 = 0, count_610 = 0, count_605 = 0, count_600 = 0, count_under_600 = 0;
    int best_value = INT_MIN, worst_value = INT_MAX, total = 0;

    // count execution time
    auto start = chrono::steady_clock::now();

    ofstream ofs;
    ofs.open("output.txt");
    ofs << "Round: " << ROUND << endl;
    ofs << "Generation: " << GENERATION << endl;
    ofs << "Population: " << POPULATION << endl;
    ofs << "Max velocity: " << MAX_VELOCITY << endl;
    ofs << "Min velocity: " << MIN_VELOCITY << endl;
    ofs << "Inertia weight: " << w << endl;
    ofs << "Particle best weight: " << c1 << endl;
    ofs << "Globe best weight: " << c2 << endl;
    ofs << "============================" << endl;

    for (int j = 0; j < ROUND; j++) {
        initialize();
        for (int i = 0; i < GENERATION; i++) {
            calcFitness();
            update();
            move();
        }
        if (gBestFitness == 620) {
            count_620++;
        } else if (gBestFitness >= 615 && gBestFitness < 620) {
            count_615++;
        } else if (gBestFitness >= 610 && gBestFitness < 615) {
            count_610++;
        } else if (gBestFitness >= 605 && gBestFitness < 610) {
            count_605++;
        } else if (gBestFitness >= 600 && gBestFitness < 605) {
            count_600++;
        } else {
            count_under_600++;
        }
        total += gBestFitness;
        if (gBestFitness > best_value) {
            best_value = gBestFitness;
        } else if (gBestFitness < worst_value) {
            worst_value = gBestFitness;
        }
        ofs << "Round: " << j + 1 << " / " << ROUND << " --- " << gBestFitness << endl;
    }

    ofs << "============================" << endl;
    ofs << "      620 :" << setw(3) << count_620 << " time(s)" << endl;
    ofs << "[615, 620):" << setw(3) << count_615 << " time(s)" << endl;
    ofs << "[610, 615):" << setw(3) << count_610 << " time(s)" << endl;
    ofs << "[605, 610):" << setw(3) << count_605 << " time(s)" << endl;
    ofs << "[600, 605):" << setw(3) << count_600 << " time(s)" << endl;
    ofs << "under 600 :" << setw(3) << count_under_600 << " time(s)" << endl;
    ofs << "============================" << endl;
    ofs << "Average value: " << (float) total / ROUND << endl;
    ofs << "Worst value: " << worst_value << endl;
    ofs << "Best value: " << best_value << endl;
    ofs << "============================" << endl;

    // count execution time
    auto end = chrono::steady_clock::now();
    cout << "Time taken: " << chrono::duration<double>(end - start).count() << " s" << endl;

    ofs << "Time taken: " << chrono::duration<double>(end - start).count() << " s" << endl;
    ofs.close();
}