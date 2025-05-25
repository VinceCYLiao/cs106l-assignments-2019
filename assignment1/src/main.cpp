/*
 * TODO: complete this file comment.
 */
#include <iostream>
#include "SimpleGraph.h"
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
#include <chrono>
#include <thread>

using namespace std;

void Welcome();

const double kattract = 0.001;

const double krepel = 0.001;

std::ifstream promptForIfstream() {
    while(true) {
        std::cout << "Enter the filename : " << std::endl;
        std::ifstream ifs;
        std::string filename {};
        std::getline(std::cin, filename);
        ifs.open(filename);
        if(!ifs.fail()) {
            return ifs;
        }
        std::cout << "File does not exist." << std::endl;
    }
}

int promptForPositiveInt() {
    while (true) {
        std::cout << "Enter the number of microseconds to run the algorithm : " << std::endl;
        int i;
        std::string input;
        std::getline(std::cin, input);
        std::stringstream ss(input);
        if(ss >> i && i > 0) {
            return i;
        }
        std::cout << "Input is not valid. Try again." << std::endl;
        ss.flush();
    }
}

SimpleGraph initSimpleGraph(std::ifstream& ifs) {
    const double kPi = 3.14159265358979323;
    SimpleGraph sg{};
    int nodeNumber;
    ifs >> nodeNumber;
    sg.nodes.resize(nodeNumber);
    size_t start, end;
    while(ifs >> start >> end) {
        sg.edges.push_back(Edge{ start, end });
    }
    for(int i = 0; i < nodeNumber; ++i) {
        sg.nodes[i] = {
            .x = cos(2*kPi*i/nodeNumber),
            .y = sin(2*kPi*i/nodeNumber),
        };
    }
    return sg;
}


void updateDeltaForAttraction(SimpleGraph& sg, std::vector<std::pair<double, double>>& delta) {
    for (size_t i = 0; i < sg.edges.size(); ++i) {
        auto& [e1, e2] = sg.edges.at(i);
        auto& node1 = sg.nodes.at(e1);
        auto& node2 = sg.nodes.at(e2);
        auto [x0, y0] = node1;
        auto [x1, y1] = node2;
        auto dy = y1 - y0;
        auto dx = x1 - x0;
        double fattract = kattract * (dy * dy + dx * dx);
        double theta = atan2(dy, dx);
        delta[e1].first += fattract * cos(theta);
        delta[e1].second += fattract * sin(theta);
        delta[e2].first -= fattract * cos(theta);
        delta[e2].second -= fattract * sin(theta);
    }
}

void updateDeltaForRepulsive(SimpleGraph& sg, std::vector<std::pair<double, double>>& delta) {
    for(size_t i = 0; i < sg.nodes.size(); ++i) {
        for (size_t j = i+1; j < sg.nodes.size(); ++j) {
                auto& node1 = sg.nodes.at(i);
                auto& node2 = sg.nodes.at(j);
                auto [x0, y0] = node1;
                auto [x1, y1] = node2;
                auto dy = y1 - y0;
                auto dx = x1 - x0;
                double distance = sqrt(dy * dy + dx * dx);
                if(distance > 0.000001) {
                    double frepel = krepel / distance;
                    double theta = atan2(dy, dx);
                    delta[i].first -= frepel * cos(theta);
                    delta[i].second -= frepel * sin(theta);
                    delta[j].first += frepel * cos(theta);
                    delta[j].second  += frepel * sin(theta);
                }
        }

    }
}


void updatePosition(SimpleGraph& sg) {
    std::vector<std::pair<double, double>> delta(sg.nodes.size(), {0.0, 0.0});
    updateDeltaForAttraction(sg, delta);
    updateDeltaForRepulsive(sg, delta);
    for(size_t i = 0; i < sg.nodes.size(); ++i) {
        sg.nodes[i].x += delta[i].first;
        sg.nodes[i].y += delta[i].second;
    }
}


// Main method
int main() {
    Welcome();
    while(true) {
        int timeLimit { promptForPositiveInt() };
        std::ifstream i { promptForIfstream() };
        SimpleGraph sg { initSimpleGraph(i) };

        InitGraphVisualizer(sg);
        DrawGraph(sg);
        while(timeLimit > 0) {
            auto start = std::chrono::high_resolution_clock::now();

            updatePosition(sg);

            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            int microseconds = elapsed.count();
            timeLimit -= microseconds;
            DrawGraph(sg);
            std::this_thread::sleep_for(std::chrono::microseconds(100));
        }
        std::cout << "Do you want to try another file ?" << std::endl;
        std::string answer;
        std::getline(std::cin, answer);
        if(answer == "yes" || answer == "y") {
            continue;
        }
        std::cout << "Bye." << std::endl;
        break;
    }
    return 0;
}


/* Prints a message to the console welcoming the user and
 * describing the program. */
void Welcome() {
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}


