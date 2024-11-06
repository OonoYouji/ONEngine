#pragma once

#include <functional>
#include <memory>
#include <vector>

class Enemy;
namespace EnemyBehaviorTree{
	enum class Status{
		SUCCESS,
		FAILURE,
		RUNNING
	};

	class Node{
	public:
		Node(Enemy* enemy):enemy_(enemy){}
		virtual ~Node() = default;
		virtual Status tick() = 0;
	protected:
		Enemy* enemy_;
	};

	class Action : public Node{
	public:
		Action(Enemy* enemy):Node(enemy){}
		virtual Status tick() = 0;
	};

	class Condition : public Node{
	public:
		Condition(Enemy* enemy):Node(enemy){}
		virtual Status tick() = 0;
	};

	class Sequence : public Node{
	public:
		Sequence(Enemy* enemy):Node(enemy){}

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
	private:
		std::vector<std::unique_ptr<Node>> children;
		size_t currentChild = 0;
	public:
		void addChild(std::unique_ptr<Node> child){
			children.push_back(std::move(child));
		}
	};

	class Selector : public Node{
	public:
		Selector(Enemy* enemy):Node(enemy){}
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
	private:

		std::vector<std::unique_ptr<Node>> children;
		size_t currentChild = 0;
	public:
		void addChild(std::unique_ptr<Node> child){
			children.push_back(std::move(child));
		}
	};

	class ScoringSelectorNode : public Node{
	public:
		ScoringSelectorNode(Enemy* enemy):Node(enemy){}

		void addChild(std::unique_ptr<Node> child,std::function<float()> scoreFunction){
			children.emplace_back(std::move(child),scoreFunction);
		}

		Status tick() override{
			Node* bestChild = nullptr;
			float highestScore = -std::numeric_limits<float>::infinity();

			// 各ノードのスコアを計算し、最高スコアのノードを選ぶ
			for(auto& [child,scoreFunction] : children){
				float score = scoreFunction();
				if(score > highestScore){
					highestScore = score;
					bestChild = child.get();
				}
			}

			// 最高スコアのノードがあれば、そのノードを実行
			if(bestChild){
				return bestChild->tick();
			}

			return EnemyBehaviorTree::Status::FAILURE; // 実行可能なノードがない場合は失敗
		}

	private:
		// 子ノードとそれぞれのスコア評価関数のペアを保持
		std::vector<std::pair<std::unique_ptr<Node>,std::function<float()>>> children;
	};
}