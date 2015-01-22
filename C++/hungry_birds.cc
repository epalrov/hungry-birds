/*
 * hungry_birds.cc - example of concurrent multi-thread application
 *
 * Copyright (C) 2013 Paolo Rovelli
 *
 * Author: Paolo Rovelli <paolorovelli@yahoo.it>
 */

#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#define NUM_BIRDS 7
#define NUM_WORMS 13

class HungryBirds {
public:
	HungryBirds();
	friend class HungryBirdsBaby;
	friend class HungryBirdsParent;
private:
	int birds;
	int worms;
	std::mutex empty_lock, any_lock;
	std::condition_variable empty, any;
};

class HungryBirdsParent {
public:
	HungryBirdsParent(HungryBirds *hungry);
	void run();
	void join();
private:
	HungryBirds *hungry;
	std::thread thread;
};

class HungryBirdsBaby {
public:
	HungryBirdsBaby(HungryBirds *hungry, int bird);
	void run();
	void join();
private:
	HungryBirds *hungry;
	std::thread thread;
	int bird;
};

/*
 * Hungry Birds - environment
 */
HungryBirds::HungryBirds() : birds(NUM_BIRDS), worms(NUM_WORMS) {
	HungryBirdsParent *parent;
	HungryBirdsBaby *baby[NUM_BIRDS];

	std::cout << std::endl << " + initial dish: " << this->worms
		<< " worms" << std::endl << std::endl;

	parent = new HungryBirdsParent(this);
	for (int i = 0; i < NUM_BIRDS; i++)
		baby[i] = new HungryBirdsBaby(this, i);

	parent->join();
	for (int i = 0; i < NUM_BIRDS; i++)
		baby[i]->join();
}

/*
 * Hungry Birds - parent bird (producer)
 */
HungryBirdsParent::HungryBirdsParent(HungryBirds *hungry) {
	this->hungry = hungry;
	this->thread = std::thread(&HungryBirdsParent::run, this);
}

void HungryBirdsParent::join() {
	this->thread.join();
}

void HungryBirdsParent::run() {
	while(1) {
		std::unique_lock<std::mutex> lock(hungry->empty_lock);
		if (hungry->worms)
			hungry->empty.wait(lock);

		hungry->worms = NUM_WORMS;
		std::cout << " + parent bird brings " << hungry->worms
			<< " worms" << std::endl << std::endl;
		for (int i = 0; i < NUM_WORMS; i++)
			hungry->any.notify_one();
	}
}

/*
 * Hungry Birds - baby birds (consumers)
 */
HungryBirdsBaby::HungryBirdsBaby(HungryBirds *hungry, int bird) {
	this->bird = bird;
	this->hungry = hungry;
	this->thread = std::thread(&HungryBirdsBaby::run, this);
}

void HungryBirdsBaby::join() {
	this->thread.join();
}

void HungryBirdsBaby::run() {
	while(1) {
		std::this_thread::sleep_for(std::chrono::microseconds(
			(rand() % ((1000000*NUM_BIRDS)/NUM_WORMS + 1))));

		std::unique_lock<std::mutex> lock(hungry->any_lock);
		if (!hungry->worms)
			hungry->any.wait(lock);

		hungry->worms--;
		if (hungry->worms) {
			std::cout << " - baby bird " << bird
				<< " eats (dish: " << hungry->worms
				<< " worms)" << std::endl;
		} else {
			std::cout << " - baby bird " << bird
				<< " eats (dish: " << hungry->worms
				<< " worms) and screams"
				<< std::endl << std::endl;
			hungry->empty.notify_one();
		}
	}
}

/*
 * Hungry Birds - main program
 */
int main(int argc, char *argv[])
{
	new HungryBirds();
	return 0;
}

