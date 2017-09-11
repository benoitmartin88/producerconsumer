#include <iostream>
#include <queue>
#include <thread>
#include <cassert>


class ProducerConsumer {
public:
	using Queue = std::queue<uint64_t>;

	ProducerConsumer(const uint64_t nbProducts=100) : queue(), nbProducts(nbProducts) {
		std::cout << "ProducerConsumer::ProducerConsumer() : nbProducts=" << nbProducts << std::endl;
	    
	    produce();
	    consume();
	}
	
	~ProducerConsumer() {
	    std::cout << "ProducerConsumer::~ProducerConsumer() Total produced: " << nbProducedSum << std::endl;
	    std::cout << "ProducerConsumer::~ProducerConsumer() Total consumed: " << nbConsumedSum << std::endl;
	    assert(nbProducedSum == nbConsumedSum);
	}

private:
	Queue queue;
    const uint64_t nbProducts;
	uint8_t nbFinishedProducer=0;
	uint64_t nbConsumedSum=0;
	uint64_t nbProducedSum=0;
	
	
    void produce() {
        std::cout << "produce()" << std::endl;
        
        for(uint64_t i=0; i<nbProducts; ++i) {
            #ifndef NDEBUG
            std::cout << "produce() push() : " << i << std::endl;
            #endif
            queue.push(i);
            ++nbProducedSum;
	        
	        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//	        consume();
        }
        
        std::cout << "produce() done !" << std::endl;
    }

    void consume() {
        std::cout << "consume()" << std::endl;
        
        while(!queue.empty()) {
            #ifndef NDEBUG
            std::cout << "consume() pop() : " << queue.front() << std::endl;
            #endif
            queue.pop();
            ++nbConsumedSum;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        std::cout << "consume() done !" << std::endl;
    }
};


int main() {

	ProducerConsumer producerConsumer(10);

	return 0;
}
