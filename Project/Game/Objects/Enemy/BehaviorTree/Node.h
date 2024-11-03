#pragma once

#include <memory>
#include <vector>

class Enemy;
namespace EnemyBehaviorTree{
	class Node{
	public:
		enum class Status{ SUCCESS,FAILURE,RUNNING };
		Node(Enemy* enemy):enemy_(enemy){}
			virtual ~Node() = default;
		virtual Status tick() = 0;
	protected:
		Enemy* enemy_;
	};

	class Action : public Node{
	public:
		virtual Status tick() = 0;
	};

	class Condition : public Node{
	public:
		virtual Status tick() = 0;
	};

	class Sequence : public Node{
	private:
		std::vector<std::unique_ptr<Node>> children;
		size_t currentChild = 0;

	public:
		void addChild(std::unique_ptr<Node> child){
			children.push_back(std::move(child));
		}

		Status tick() override{
			while(currentChild < children.size()){
				Status status = children[currentChild]->tick();
				if(status == Status::FAILURE){
					currentChild = 0;
					return Status::FAILURE;
				}
				if(status == Status::RUNNING){
					return Status::RUNNING;
				}
				++currentChild;
			}
			currentChild = 0;
			return Status::SUCCESS;
		}
	};

	class Selector : public Node{
	private:
		std::vector<std::unique_ptr<Node>> children;
		size_t currentChild = 0;

	public:
		void addChild(std::unique_ptr<Node> child){
			children.push_back(std::move(child));
		}

		Status tick() override{
			while(currentChild < children.size()){
				Status status = children[currentChild]->tick();
				if(status == Status::SUCCESS){
					currentChild = 0;
					return Status::SUCCESS;
				}
				if(status == Status::RUNNING){
					return Status::RUNNING;
				}
				++currentChild;
			}
			currentChild = 0;
			return Status::FAILURE;
		}
	};
}
