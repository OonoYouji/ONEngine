
struct Test {
	float tmp;
};


ConstantBuffer<Test> gTest : register(b0);


[numthreads(1, 1, 1)]
void main() {
	float a = gTest.tmp;

}