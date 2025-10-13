#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <functional>

using namespace std;

enum{
  ADD, SUB, OR, NOR, AND, NAND, XOR, XNOR, SHR, BIZ, BNZ, BIC, BNC, BRK=63
// 0    1   2    3    4    5     6    7     8    9    10   11   12   63
};

class DynamicPointerVector {
private:
    vector<int> registers;
    vector<int> ram;
    vector<int> stack;
    
    int addr;  // Current RAM address
    int sp;    // Current stack pointer
    
    // Vector of functions that return pointers to int
    vector<function<int*()>> pointer_getters;

public:
    DynamicPointerVector() : 
        registers(10), 
        ram(16777216), 
        stack(256), 
        addr(0), 
        sp(0) 
    {
        // Initialize pointer getters for registers (indices 0-9)
        for (int i = 0; i < 10; i++) {
            // Capture 'i' by value to create a fixed reference to each register
            pointer_getters.push_back([this, i]() -> int* { return &registers[i]; });
        }
        
        // Initialize pointer getter for RAM (index 10)
        pointer_getters.push_back([this]() -> int* { return &ram[addr]; });
        
        // Initialize pointer getter for stack (index 11)
        pointer_getters.push_back([this]() -> int* { return &stack[sp]; });
    }
    
    // Get a pointer based on index
    int* get_pointer(int index) {
        if (index < 0 || index >= pointer_getters.size()) {
            return nullptr;  // Index out of bounds
        }
        return pointer_getters[index]();
    }
    
    // Set the current RAM address
    void set_addr(int new_addr) {
        if (new_addr >= 0 && new_addr < ram.size()) {
            addr = new_addr;
        }
    }
    
    // Set the current stack pointer
    void set_sp(int new_sp) {
        if (new_sp >= 0 && new_sp < stack.size()) {
            sp = new_sp;
        }
    }
    
    // Get current address
    int get_addr() const { return addr; }
    
    // Get current stack pointer
    int get_sp() const { return sp; }

};


vector<int> instInput(16777216, 0), in1Input(16777216, 0), in2Input(16777216, 0), outInput(16777216, 0);
int pc = 0;
bool halt = false, isCarry, isZero;

int emulate(int counter, int inst, int in1, int in2, int out, DynamicPointerVector &mem) {
  int reg7 = *mem.get_pointer(7);
  int reg8 = *mem.get_pointer(8);
  int reg9 = *mem.get_pointer(9);
  mem.set_addr(reg7 + reg8*256 + reg9*65536);
  cout<<"addr: "<<mem.get_addr()<<endl;
  cout<<"counter: "<<counter<<endl;

  *mem.get_pointer(0) = 0;
  int inAddr1 = in1;
  int inAddr2 = in2;
  if ((inst & 128) != 128) in1 = *mem.get_pointer(in1);
  if ((inst & 64) != 64) in2 = *mem.get_pointer(in2);
  cout << "Instruction: " << inst << ", in1: " << in1 << ", in2: " << in2 << ", out: " << out << endl;

  // Handle stack writes - increment SP before operation
  if (out == 11) {
    int current_sp = mem.get_sp();
    if (current_sp < 255) {
      mem.set_sp(current_sp + 1);
    }
  }
  
  int result;
  switch (inst & 63) {
  case ADD:
    *mem.get_pointer(out) = in1 + in2;
    result = *mem.get_pointer(out);
    if (result == 0) isZero = true;
    else isZero = false;
    if (result > 255) {
      result %= 256;
      *mem.get_pointer(out) = result;
      isCarry = true;
    } else isCarry = false;
    cout<<"Added "<<*mem.get_pointer(out)<<endl;
    break;
  case SUB:
    *mem.get_pointer(out) = in1 - in2;
    result = *mem.get_pointer(out);
    cout<<"Sub zf: "<<result<<endl;
    if (result == 0) isZero = true;
    else isZero = false;
    if (result < 0) {
      isCarry = false;
    } else isCarry = true;
    cout<<"carry: "<<isCarry<<", zero: "<<isZero<<endl;
    result %= 256;
    *mem.get_pointer(out) = result;
    cout<<"Subtracted "<<*mem.get_pointer(out)<<endl;
    break;
  case OR:
    *mem.get_pointer(out) = in1 | in2;
    result = *mem.get_pointer(out);
    if (result == 0) isZero = true;
    else isZero = false;
    isCarry = false;
    result %= 256;
    *mem.get_pointer(out) = result;
    cout<<"Or "<<*mem.get_pointer(out)<<endl;
    break;
  case NOR:
    *mem.get_pointer(out) = 255-(in1 | in2);
    result = *mem.get_pointer(out);
    if (result == 0) isZero = true;
    else isZero = false;
    isCarry = true;
    result %= 256;
    *mem.get_pointer(out) = result;
    cout<<"Nor "<<*mem.get_pointer(out)<<endl;
    break;
  case AND:
    *mem.get_pointer(out) = in1 & in2;
    result = *mem.get_pointer(out);
    if (result == 0) isZero = true;
    else isZero = false;
    isCarry = true;
    result %= 256;
    *mem.get_pointer(out) = result;
    cout<<"And "<<*mem.get_pointer(out)<<endl;
    break;
  case NAND:
    *mem.get_pointer(out) = 255-(in1 & in2);
    result = *mem.get_pointer(out);
    if (result == 0) isZero = true;
    else isZero = false;
    isCarry = false;
    result %= 256;
    *mem.get_pointer(out) = result;
    cout<<"Nand "<<*mem.get_pointer(out)<<endl;
    break;
  case XOR:
    *mem.get_pointer(out) = in1 ^ in2;
    result = *mem.get_pointer(out);
    if (result == 0) isZero = true;
    else isZero = false;
    isCarry = true;
    result %= 256;
    *mem.get_pointer(out) = result;
    cout<<"Xor "<<*mem.get_pointer(out)<<endl;
    break;
  case XNOR:
    *mem.get_pointer(out) = 255-(in1 ^ in2);
    result = *mem.get_pointer(out);
    if (result == 0) isZero = true;
    else isZero = false;
    isCarry = true;
    result %= 256;
    *mem.get_pointer(out) = result;
    cout<<"Xnor "<<*mem.get_pointer(out)<<endl;
    break;
  case SHR:
    *mem.get_pointer(out) = floor((in1 + in2)/2);
    result = *mem.get_pointer(out);
    if (result == 0) isZero = true;
    else isZero = false;
    isCarry = false;
    result %= 256;
    *mem.get_pointer(out) = result;
    cout<<"Shr "<<*mem.get_pointer(out)<<endl;
    break;

  case BIZ:
    cout<<"BIZ: "<<isZero<<endl;
    if (isZero) {
      return mem.get_addr();
    }
    break;
  case BNZ:
    if (!isZero) return mem.get_addr();
    break;
  case BIC:
    if (isCarry) return mem.get_addr();
    break;
  case BNC:
    if (!isCarry) return mem.get_addr();
    break;
  case BRK:
    halt = true;
    break;
  }

  // After operation, adjust stack pointer for reads
  if ((((inst & 128) != 128) && (inAddr1 == 11)) || (((inst & 64) != 64) && (inAddr2 == 11))) {
    int current_sp = mem.get_sp();
    cout<<"Stack: "<<mem.get_sp()<<endl;
    if (current_sp > 0) {
      mem.set_sp(current_sp - 1);
    }
  }
  return counter+1;
}


int main(int argc, char* argv[]) {

  // File opening
  if (argc < 2) {
    cout<<"Wrong input format"<<endl;
    return 1;
  }
  ifstream file(argv[1]);
  if (!file.is_open()) {
    cerr << "Error: Could not open file '" << argv[1] <<"'"<< endl;
    return 1;
  }

  int inputLen;
  file >> inputLen;
  
  // Read the next inputLen lines, each containing 4 integers
  for (int i = 0; i < inputLen; i++) {
    if (!(file >> instInput[i] >> in1Input[i] >> in2Input[i] >> outInput[i])) {
      cerr << "Error: Failed to read integers from line " << i+1 << endl;
      return 1;
    }
  }
  DynamicPointerVector mem;
  mem.set_sp(0);
  while (!halt) pc = emulate(pc, instInput[pc], in1Input[pc], in2Input[pc], outInput[pc], mem);

}

/* start: 0
     add 4 0 reg1
     add 5 0 reg2
     add 0 0 reg3
   mul: 3
     add end.Low 0 reg7
     add end.Mid 0 reg8
     add end.High 0 reg9
     sub reg2 0 reg0
     biz
     add reg3 reg1 reg3
     sub reg2 1 reg2
     add mul.Low 0 reg7
     add mul.Mid 0 reg8
     add mul.High 0 reg9
     add 1 0 reg0
     bnz
   end: 15
     brk
*/
