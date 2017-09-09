#include <iostream>
#include <queue>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>


// TODO mutex protected std::cout

template<typename _Queue> class Producer {
public:
	Producer() = delete;
	Producer(_Queue& queue, std::mutex& mutex, const uint64_t nbProducts=1000)
			: id(ID++), queue(queue), mutex(mutex), nbProducts(nbProducts), nbEffectiveProduced(1), productionThread(&Producer<_Queue>::produce, this), cv() {
		std::cout << "Producer::Producer() id=" << id << std::endl;
		productionThread.detach();  // run thread in background
	}
	~Producer() noexcept {
		std::cout << "Producer::~Producer() id=" << id << ", nbProducts=" << nbEffectiveProduced << std::endl;
		
		std::unique_lock<std::mutex> lock(mutex);
		destroy = true;
		cv.wait(lock, [&]{return isFinished;});
	}

	void produce() {
		for(; !destroy && nbEffectiveProduced<=nbProducts; ++nbEffectiveProduced) {
		    {
		        std::lock_guard<std::mutex> lock(mutex);
#ifndef NDEBUG
//		        std::cout << "Producer::produce() id=" << id << " push() : " << nbEffectiveProduced << std::endl;
#endif
			    queue.push(nbEffectiveProduced);
		    }
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		
		std::lock_guard<std::mutex> lock(mutex);
		isFinished = true;
		cv.notify_one();
	}
	
	uint64_t getNbProducts() const {
	    return nbEffectiveProduced;
	}

private:
	static int ID;
	const int id;
	_Queue& queue;
	std::mutex& mutex;
	const uint64_t nbProducts;
	uint64_t nbEffectiveProduced;
	std::thread productionThread;
	std::condition_variable cv;
	bool destroy = false;
	bool isFinished = false;
};
template<typename _Queue> int Producer<_Queue>::ID = 0;


/**
    Spin-lock consumer
*/
template<typename _Queue> class Consumer {
public:
	Consumer() = delete;
	Consumer(_Queue& queue, std::mutex& mutex)
			: id(ID++), queue(queue), nbProducts(0), mutex(mutex), consumerthread(&Consumer<_Queue>::consume, this), cv() {
		std::cout << "Consumer::Consumer() id=" << id << std::endl;
		consumerthread.detach();    // run thread in background
	}
	~Consumer() noexcept {
		std::cout << "Consumer::~Consumer() id=" << id << ", nbProducts=" << nbProducts << std::endl;
		
		std::unique_lock<std::mutex> lock(mutex);
		destroy = true;
		cv.wait(lock, [&]{return isFinished;});
	}

	void consume() {
		while(!destroy) {
		    {
        		std::lock_guard<std::mutex> lock(mutex);
		        if(queue.size() > 0) {
#ifndef NDEBUG
//		            std::cout << "Consumer::consume() id=" << id << " pop() : " << queue.front() << std::endl;
#endif
			        queue.pop();
			        ++nbProducts;
		        }
		    }
		}
		
		std::lock_guard<std::mutex> lock(mutex);
		isFinished = true;
		cv.notify_one();
	}
	
	uint64_t getNbProducts() const {
	    return nbProducts;
	}

private:
	static int ID;
	const int id;
	_Queue& queue;
	uint64_t nbProducts;
	std::mutex& mutex;
	std::thread consumerthread;
	std::condition_variable cv;
	bool destroy = false;
	bool isFinished = false;
};
template<typename _Queue> int Consumer<_Queue>::ID = 0;


class ProducerConsumer {
public:
	using Queue = std::queue<uint64_t>;
	using Producer = ::Producer<Queue>;
	using Consumer = ::Consumer<Queue>;

	ProducerConsumer(const uint8_t nbProducer=1, const uint8_t nbConsumer=1) : queue(), mutex(), nbProducer(nbProducer), nbConsumer(nbConsumer), producers(nbProducer), consumers(nbConsumer) {
		std::cout << "ProducerConsumer::ProducerConsumer() : "
				<< "nbProducer=" << int64_t(nbProducer)
				<< ", nbConsumer=" << int64_t(nbConsumer) << std::endl;
				
		std::cout << "ProducerConsumer::ProducerConsumer() : starting consumers" << std::endl;
		for(uint8_t i=0; i<nbConsumer; ++i) {
			consumers.push_front(std::unique_ptr<Consumer>(new Consumer(queue, mutex)));
		}
		
		std::cout << "ProducerConsumer::ProducerConsumer() : starting producers" << std::endl;
		for(uint8_t i=0; i<nbProducer; ++i) {
			producers.push_front(std::unique_ptr<Producer>(new Producer(queue, mutex)));
		}
	}
	
	~ProducerConsumer() {
	    uint64_t nbProducedSum = 0;
	    for(uint8_t i=0; i<nbProducer; ++i) {
    	    nbProducedSum += producers[i]->getNbProducts();
	    }
	    
	    uint64_t nbConsumedSum = 0;
	    for(uint8_t i=0; i<nbConsumer; ++i) {
    	    nbConsumedSum += consumers[i]->getNbProducts();
	    }
	    
	    std::cout << "ProducerConsumer::~ProducerConsumer() Total produced: " << nbProducedSum << std::endl;
	    std::cout << "ProducerConsumer::~ProducerConsumer() Total consumed: " << nbConsumedSum << std::endl;
	    assert(nbProducedSum == nbConsumedSum);
	}

private:
	Queue queue;
	std::mutex mutex;
	const uint8_t nbProducer;
    const uint8_t nbConsumer;
	std::deque<std::unique_ptr<Producer>> producers;
	std::deque<std::unique_ptr<Consumer>> consumers;
};


int main() {

	ProducerConsumer producerConsumer(2, 2);
	
	std::this_thread::sleep_for(std::chrono::seconds(2));

	return 0;
}
