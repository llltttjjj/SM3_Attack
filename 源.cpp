#include<iostream>
using namespace std;
uint64_t pow(int n) {
	if (n == 0)
		return 1;
	return 2 * pow(n - 1);
}
class sm3_context {
private:
	uint32_t* info;     //unknown bits message
	uint64_t size;     //size of message
	unsigned int messlen;
	uint32_t dig[8];     //256 bits digest
	const uint64_t U32 = pow(32);
	void sm3_init(const uint32_t* message) {
		uint64_t tmp = (size - 1) / 32 + 1;
		for (uint64_t i = 0; i < tmp; i++)
			info[i] = message[i];
		for (uint64_t i = tmp; i < messlen; i++)
			info[i] = 0;
		if (size % 32 != 0)     //在消息最后一位置一
			info[tmp - 1] += pow(31 - size % 32);
		else
			info[tmp] += U32 / 2;
		info[messlen - 2] = size / U32;
		info[messlen - 1] = size % U32;
	}
	uint32_t ls(const uint32_t val, int num) { return val >> (32 - num) | (val << num); }
	uint32_t P0(const uint32_t val) { return val ^ ls(val, 9) ^ ls(val, 17); }
	uint32_t P1(const uint32_t val) { return val ^ ls(val, 15) ^ ls(val, 23); }
	uint32_t T(short j) { if (j < 16) return 0x79CC4519; return 0x7A879D8A; }
	uint32_t FF(uint32_t X, uint32_t Y, uint32_t Z, short j) {
		if (j < 16) return X ^ Y ^ Z;
		return (X & Y) | (X & Z) | (Y & Z);
	}
	uint32_t GG(uint32_t X, uint32_t Y, uint32_t Z, short j) {
		if (j < 16) return X ^ Y ^ Z;
		return (X & Y) | (~X & Z);
	}
	//message is 512 bits, IV is 256 bits
	void itercompress(const uint32_t* message, const uint32_t* IV, uint32_t* output) {
		uint32_t W[68];
		uint32_t W_[64];
		for (int j = 0; j < 16; j++)
			W[j] = message[j];
		for (int j = 16; j < 68; j++)
			W[j] = P1(W[j - 16] ^ W[j - 9] ^ ls(W[j - 3], 15)) ^ ls(W[j - 13], 7) ^ W[j - 6];
		for (int j = 0; j < 64; j++)
			W_[j] = W[j] ^ W[j + 4];
		uint32_t SS1, SS2, TT1, TT2;
		for (int i = 0; i < 8; i++)
			output[i] = IV[i];
		uint32_t& A = output[0];
		uint32_t& B = output[1];
		uint32_t& C = output[2];
		uint32_t& D = output[3];
		uint32_t& E = output[4];
		uint32_t& F = output[5];
		uint32_t& G = output[6];
		uint32_t& H = output[7];
		for (int j = 0; j < 64; j++) {
			SS1 = ls(ls(A, 12) + E + ls(T(j), j), 7);
			SS2 = SS1 ^ ls(A, 12);
			TT1 = FF(A, B, C, j) + D + SS2 + W_[j];
			TT2 = GG(E, F, G, j) + H + SS1 + W[j];
			D = C;
			C = ls(B, 9);
			B = A;
			A = TT1;
			H = G;
			G = ls(F, 19);
			F = E;
			E = P0(TT2);
		}
		for (int i = 0; i < 8; i++)
			output[i] = IV[i] ^ output[i];
	}
public:
	sm3_context(uint64_t sizeOfMessage) {
		messlen = ((sizeOfMessage - 447) / 512 + 2) * 16;
		info = new uint32_t[messlen];
		size = sizeOfMessage;
		dig[0] = 0x7380166F;
		dig[1] = 0x4914B2B9;
		dig[2] = 0x172442D7;
		dig[3] = 0xDA8A0600;
		dig[4] = 0xA96F30BC;
		dig[5] = 0x163138AA;
		dig[6] = 0xE38DEE4D;
		dig[7] = 0xB0FB0E4E;
	}
	void sm3_hash(const uint32_t* message,uint32_t* output) {
		sm3_init(message);
		for (int i = 0; i < messlen / 16; i++) {
			itercompress(&info[i * 16], dig, output);
			for (int j = 0; j < 8; j++)
				dig[i] = output[i];
		}
	}
	~sm3_context() {
		delete[] info;
	}
};
void naiveBirthdayAttack() {
	uint32_t x = pow(18);     //try 2^18 message to find a collision, much bigger than 2^16
	uint32_t* buffer = new uint32_t[x];
	uint32_t output[8];
	sm3_context ctx(32);
	for (uint32_t t = 0; t < x; t++) {
		ctx.sm3_hash(&t, output);
		buffer[t] = output[0];     //only take 32 bits of the digest
		for (uint32_t i = 0; i < t; i++)
			if (buffer[i] == buffer[t]) {
				cout << "Find a collision!" << endl
					<< "Message 1: " << i << endl
					<< "Message 2: " << t << endl;
				return;
			}
	}
	cout << "No collision is found in buffer!" << endl;
	delete[] buffer;
	return;
}

#include<stdlib.h>
#include<ctime>
const int SIZE = 33554432;
static uint64_t mem[SIZE]{};     //0x7fffffff bits array, set to 0
bool inSet(uint64_t* mem, uint64_t* n) {
	for (uint32_t i = 0; i < SIZE; i++)
		if (mem[i] == *n)
			return 1;
	return 0;
}
bool success;
void* rhoCollisionAttack(void* n) {
	cout << "No." << (uint32_t)n << " Thread Start.\n";
	uint64_t x, y;
	sm3_context ctx_x(64);
	sm3_context ctx_y(64);
	uint32_t outx[8], outy[8];
	srand(clock());
	uint32_t m = (uint32_t)n * 2097152;
	for (uint64_t i = 0; i < pow(21); i++) {
		x = rand();
		mem[i + m] = x;
		while (y != x && (!inSet(mem, &x))) {

			}
		}
	}
	cout << "No collision found!" << endl;
	return nullptr;
}
#define HAVE_STRUCT_TIMESPEC
#include<pthread.h>
void multiThreadRho() {
	success = 0;
	pthread_t thr[16];
	for (int i = 0; i < 16; i++) {
		pthread_create(&thr[i], NULL, rhoCollisionAttack, (void*)i);
	}
	while (!success)
		for (int i = 0; i < 16; i++)
			pthread_exit(&thr[i]);
	delete[] mem;
}
int main() {
	/* A Simple Test of SM3
	char message[] = "dfsegsfcfdsfvxcsdfd43sefsrt435fdsgds";
	uint32_t output[8];
	sm3_context ctx(296);
	ctx.sm3_hash((uint32_t*)message, output);
	for (int i = 0; i < 8; i++)
		cout << output[i];
	*/
	//naiveBirthdayAttack();
	/*
	Message 1: 26373
	Message 2: 58539
	(2^16 = 65536)
	*/
	multiThreadRho();
	return 0;
}