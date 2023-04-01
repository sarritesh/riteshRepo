#include <iostream>
#include <string>

struct Verbose {
    std::string id_ = "nameless";

    void rename(std::string id) {
        std::cout << "   Verbose rename from " << id_ << " to " << id << "\n";
        id_ = std::move(id);
    }

    Verbose() {
        std::cout << "   Verbose default construct " << id_ << "\n";
    }

    Verbose(Verbose const& o) {
        std::cout << "   Verbose copy construct " << id_ << "<=" << o.id_ << "\n";
    }

    Verbose(Verbose&& o) {
        std::cout << "   Verbose move construct " << id_ << "<=" << o.id_ << "\n";
    }

    ~Verbose() {
        std::cout << "   Verbose destruct " << id_ << "\n";
    }

    Verbose& operator=(Verbose const& o) {
        std::cout << "   Verbose copy assign " << id_ << "<=" << o.id_ << "\n";
        return *this;
    }

    Verbose& operator=(Verbose&& o) {
        std::cout << "   Verbose move assign " << id_ << "<=" << o.id_ << "\n";
        return *this;
    }
};

template <typename T>
struct SharedState {
public:
    SharedState(T* ptr): ptr_(ptr), count_(1) {
        std::cout << "  SharedState construct " << id() << "\n";
    }

    ~SharedState() {
        std::cout << "  SharedState destruct " << id() << ", pointer to be deleted " << ptr_ << "\n";
        delete ptr_;
    }

    SharedState(SharedState const& o) = delete;
    SharedState(SharedState&& o) = delete;
    SharedState& operator=(SharedState const& o) = delete;
    SharedState& operator=(SharedState&& o) = delete;

    std::size_t increase() {

		std::cout<<"reference count is now "<< count_ +1 << "\n";
		
        return ++count_;
    }

    std::size_t decrese() {
		if(count_ == 1 )
		{
			std::cout<<"reference count is now reduced to 0 \n";
		}
        return --count_;
    }

    T* get() const {
        return ptr_;
    }

    std::string id() const {
        return "ref_count$" + std::to_string(count_);
    }

private:
    T* ptr_;
    std::size_t count_;
};

template <typename T>
class SharedPtr {
public:
    std::string id() const {
        using namespace std::literals;
        return id_ + "->" + (state_ ? state_->id() : "nullptr"s);
    }

    void rename(std::string id) {
        std::cout << " SharedPtr rename from " << this->id_ << " to " << id << "\n";
        id_ = std::move(id);
    }

    SharedPtr(): state_(nullptr)  {
        std::cout << " SharedPtr default construct " << id() << "\n";
    }

    SharedPtr(T* ptr): state_(new SharedState<T>(ptr)) {
        std::cout << " SharedPtr direct construct " << id() << "\n";
    }

    SharedPtr(SharedPtr const& o) {
        std::cout << " SharedPtr copy construct " << id() << "<=" << o.id() << "\n";
        setSharedState(o.state_);
        std::cout << " ======================== " << id() << "<=" << o.id() << "\n";
    }

    SharedPtr(SharedPtr&& o) {
        std::cout << " SharedPtr move construct " << id() << "<=" << o.id() << "\n";
        setSharedState(o.state_);
        o.removeSharedState();
        std::cout << " ======================== " << id() << "<=" << o.id() << "\n";
    }

    ~SharedPtr() {
        std::cout << " SharedPtr destruct " << id() << "\n";
        removeSharedState();
    }

    SharedPtr& operator=(SharedPtr const& o) {
        std::cout << " SharedPtr copy assign " << id() << "<=" << o.id() << "\n";
        removeSharedState();
        setSharedState(o.state_);
        std::cout << " ===================== " << id() << "<=" << o.id() << "\n";
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& o) {
        std::cout << " SharedPtr move assign " << id() << "<=" << o.id() << "\n";
        removeSharedState();
        setSharedState(o.state_);
        o.removeSharedState();
        std::cout << " ===================== " << id() << "<=" << o.id() << "\n";
        return *this;
    }

    T* get() const {
        return state_ ? state_->get() : nullptr;
    }

    T* operator->() const {
        if (!state_) {
            throw std::logic_error("dereference nullptr");
        }
        return state_->get();
    }

private:
    void removeSharedState() {
        if (state_) {
            if(state_->decrese() == 0) 
			{ // Every SharedPtr has it own SharedState<T>* state_ so when a shared pointer is constructed the state reference count is 
			  // incremented or decrement when deleted moved from a temp object for temp object and incremented for the new object. 						
                delete state_;
            }
            state_ = nullptr;
        }
    }

    void setSharedState(SharedState<T>* state) {
        state_ = state;
        if (state_) {
            state_->increase();
        }
    }

    std::string id_ = "Nameless";
    SharedState<T>* state_ = nullptr;
};

SharedPtr<Verbose> getVerbose()
{
    return SharedPtr<Verbose>(new Verbose());
}

int main() {
    std::cout << "Default construct A\n";
    auto A = SharedPtr<Verbose>(new Verbose);
    A->rename("a");
    A.rename("A");
    std::cout << A.id() << " points to: " << A.get() << '\n';

    std::cout << "Copy construct B from A\n";
    auto B = A;
    B.rename("B");
    std::cout << A.id() << " points to: " << A.get() << '\n';
    std::cout << B.id() << " points to: " << B.get() << '\n';

    std::cout << "Move construct C from A\n";
    auto C = std::move(A);
    C.rename("C");
    std::cout << A.id() << " points to: " << A.get() << '\n';
    std::cout << B.id() << " points to: " << B.get() << '\n';
    std::cout << C.id() << " points to: " << C.get() << '\n';
	
	{
        std::cout << "Construct D from C\n";
		auto D = C;
		D.rename("D");
        D = getVerbose();

	}
}