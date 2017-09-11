#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>


class ProducerConsumer {
public:
	using Queue = std::queue<uint64_t>;

	ProducerConsumer(const uint8_t nbProducer=1, const uint8_t nbConsumer=1, const uint64_t nbProducts=100) 
	        : queue(), mutex(), cv(), nbProducer(nbProducer), nbConsumer(nbConsumer), nbProducts(nbProducts), producers(nbProducer), consumers(nbConsumer) {
		std::cout << "ProducerConsumer::ProducerConsumer() : "
				<< "nbProducer=" << int64_t(nbProducer)
				<< ", nbConsumer=" << int64_t(nbConsumer) 
				<< ", nbProducts=" << nbProducts << std::endl;
				
	    std::cout << "ProducerConsumer::ProducerConsumer() : starting producers" << std::endl;
		for(uint8_t i=0; i<nbProducer; ++i) {
			producers.emplace_front(std::thread(&ProducerConsumer::produce, this, i));
		}

		std::cout << "ProducerConsumer::ProducerConsumer() : starting consumers" << std::endl;
		for(uint8_t i=0; i<nbConsumer; ++i) {
			consumers.emplace_front(std::thread(&ProducerConsumer::consume, this, i));
		}
	}
	
	~ProducerConsumer() {
	    for(uint8_t i=0; i<nbProducer; ++i) {
	        producers[i].join();
	    }
	    
	    for(uint8_t i=0; i<nbConsumer; ++i) {
    	    consumers[i].join();
	    }
	    
	    std::cout << "ProducerConsumer::~ProducerConsumer() Total produced: " << nbProducedSum << std::endl;
	    std::cout << "ProducerConsumer::~ProducerConsumer() Total consumed: " << nbConsumedSum << std::endl;
	    assert(nbProducedSum == nbConsumedSum);
	}

private:
	Queue queue;
	std::mutex mutex;
	std::condition_variable cv;
	const uint8_t nbProducer;
    const uint8_t nbConsumer;
    const uint64_t nbProducts;
	std::deque<std::thread> producers;
	std::deque<std::thread> consumers;
	uint8_t nbFinishedProducer=0;
	uint64_t nbConsumedSum=0;
	uint64_t nbProducedSum=0;
	
	
    void produce(const uint8_t id) {
        std::cout << "produce id=" << unsigned(id) << std::endl;
        
        for(uint64_t i=0; i<nbProducts; ++i) {
            {
                std::lock_guard<std::mutex> lock(mutex);
                #ifndef NDEBUG
                std::cout << "produce() id=" << unsigned(id) << " push() : " << i << std::endl;
                #endif
	            queue.push(i);
	            ++nbProducedSum;
	        }
	        
	        cv.notify_one();
	        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        {
            std::lock_guard<std::mutex> lock(mutex);
            ++nbFinishedProducer;
            std::cout << "produce id=" << unsigned(id) << " done !" << std::endl;
            cv.notify_all();
        }
    }

    void consume(const uint8_t id) {
        std::cout << "consume id=" << unsigned(id) << std::endl;
        uint64_t consumed=0;
        
        while(true) {
            {
                std::unique_lock<std::mutex> lock(mutex);
                cv.wait(lock, [&] {
//                    std::cout << "wait: queue.size()=" << queue.size() 
//                        << ", nbProducer=" << unsigned(nbProducer) 
//                        << ", nbFinishedProducer=" << unsigned(nbFinishedProducer) << std::endl;
                    return !queue.empty() || nbFinishedProducer==nbProducer;
                });
                
                if(nbFinishedProducer==nbProducer) {
                    break;
                }
                assert(nbFinishedProducer<nbProducer);
                assert(queue.size() > 0);
                 
                #ifndef NDEBUG
                std::cout << "consume() id=" << unsigned(id) << " pop() : " << queue.front() << std::endl;
                #endif
                queue.pop();
                ++consumed;
            }
        }
        
        {
            std::unique_lock<std::mutex> lock(mutex);
            assert(queue.size() == 0);
            nbConsumedSum += consumed;
        }
        
        std::cout << "consume id=" << unsigned(id) << " done ! consumed=" << consumed << std::endl;
    }
	
};


int main() {

	ProducerConsumer producerConsumer(2, 2, 100);

	return 0;
}
