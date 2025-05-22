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

using namespace std;

void Welcome();

const double kattract = 0.001;

const double krepel = 0.001;

std::string promptForFilename() {
    std::cout << "Enter the filename : " << std::endl;
    std::string filename {};
    std::getline(std::cin, filename);
    return filename;
}

int promptForPositiveInt() {
    while (true) {
        std::cout << "Enter the number of microseconds to run the algorithm : " << std::endl;
        int i;
        std::string input;
        std::getline(std::cin, input);
        std::stringstream ss(input);
        if(ss >> i) {
            return i;
        }
        std::cout << "Input is not valid. Try again." << std::endl;
        ss.flush();
    }
}


std::vector<string> split(stringstream& ss, char deli) {
    std::vector<string> sv{};
    for (std::string line; std::getline(ss, line, deli);) {
        sv.push_back(line);
    }
    return sv;
}

void updateNodePositionForAttraction(SimpleGraph& sg) {
    for (size_t i = 0; i < sg.edges.capacity(); ++i) {
        auto& [e1, e2] = sg.edges.at(i);
        auto& node1 = sg.nodes.at(e1);
        auto& node2 = sg.nodes.at(e2);
        auto [x0, y0] = node1;
        auto [x1, y1] = node2;
        double fattract = kattract * ((y1 - y0) * (y1 - y0) + (x1 - x0) * (x1 - x0));
        double theta = atan2(y1 - y0, x1 - x0);
        node1.x -= fattract * cos(theta);
        node1.y -= fattract * sin(theta);
        node2.x += fattract * cos(theta);
        node2.y += fattract * sin(theta);
    }
}

void updateNodePositionForRepulsive(SimpleGraph& sg) {
    for(size_t i = 0; i < sg.nodes.capacity(); ++i) {
        auto& node1 = sg.nodes.at(i);
        auto node2Index = i+1;
        auto& node2 = sg.nodes.at(node2Index == sg.nodes.capacity() ? 0 : node2Index);
        auto [x0, y0] = node1;
        auto [x1, y1] = node2;
        double frepel = krepel / sqrt((y1 - y0) * (y1 - y0) + (x1 - x0) * (x1 - x0));
        double theta = atan2(y1 - y0, x1 - x0);
        node1.x -= frepel * cos(theta);
        node1.y -= frepel * sin(theta);
        node2.x += frepel * cos(theta);
        node2.y += frepel * sin(theta);
    }
}


// Main method
int main() {
    Welcome();
    int timeLimit = promptForPositiveInt();
    std::ifstream i;
    while (true) {
        std::string filename { promptForFilename() };
        i.open(filename);
        if (!i.fail()) {
            break;
        }

    }
    std::stringstream ss;
    ss << i.rdbuf();
    auto sv = split(ss, '\n');
    int nodeNumber = std::stoi(sv.at(0));
    SimpleGraph sg {};
    std::vector<Node> nv{};
    const double kPi = 3.14159265358979323;
    for(int i = 0; i <= nodeNumber; ++i) {
        nv.push_back({
            .x = cos(2*kPi*i/nodeNumber),
            .y = sin(2*kPi*i/nodeNumber),
        });
    }
    std::vector<Edge> ev{};
    for (auto i = sv.begin() + 1; i != sv.end(); ++i) {
        stringstream edgeSS(*i);
        auto edgeSV = split(edgeSS, ' ');
        ev.push_back({
            .start = std::stoul(edgeSV.at(0)),
            .end = std::stoul(edgeSV.at(1)),
        });
    }
    sg.nodes = nv;
    sg.edges = ev;
    InitGraphVisualizer(sg);
    DrawGraph(sg);
    while(timeLimit > 0) {
        auto start = std::chrono::high_resolution_clock::now();

        updateNodePositionForAttraction(sg);
        updateNodePositionForRepulsive(sg);

        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        int millisecond = elapsed.count();
        timeLimit -= (millisecond > 0 ? millisecond : 1);
        std::cout << timeLimit << " " << millisecond << std::endl;
    }
    InitGraphVisualizer(sg);
    DrawGraph(sg);
    std::cout << "done" << std::endl;
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


