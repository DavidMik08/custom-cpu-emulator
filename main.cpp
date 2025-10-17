#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

int hexToDec(char c1, char c2) {
  int result = 0;
  if (isdigit(c1)) {
    result += 16 * (c1 - '0');
  } else {
    switch (c1) {
    case 'A': case 'a': result += 160; break;
    case 'B': case 'b': result += 176; break;
    case 'C': case 'c': result += 192; break;
    case 'D': case 'd': result += 208; break;
    case 'E': case 'e': result += 224; break;
    case 'F': case 'f': result += 240; break;
    }
  }
    
  if (isdigit(c2)) {
    result += c2 - '0';
  } else {
    switch (c2) {
    case 'A': case 'a': result += 10; break;
    case 'B': case 'b': result += 11; break;
    case 'C': case 'c': result += 12; break;
    case 'D': case 'd': result += 13; break;
    case 'E': case 'e': result += 14; break;
    case 'F': case 'f': result += 15; break;
    }
  }
  return result;
}

class memory {
public:
  vector<int> ram;
  vector<int> stack;
  vector<int> registers;
  int addr;
  int sp;
  int oldSP;
  int pc;
  int inst;
  int in1;
  int in2;
  int out;
  bool cf;
  bool zf;
  bool halt;
  
  memory() {
    ram.resize(16777216);
    stack.resize(256);
    registers.resize(9);
    pc = 0;
    sp = 0;
    halt = false;
  }
  int readFromMemory(int index) {
    switch (index) {
    case 0: return 0; break;
    case 1: return registers[0]; break;
    case 2: return registers[1]; break;
    case 3: return registers[2]; break;
    case 4: return registers[3]; break;
    case 5: return registers[4]; break;
    case 6: return registers[5]; break;
    case 7: return registers[6]; break;
    case 8: return registers[7]; break;
    case 9: return registers[8]; break;
    case 10: return ram[addr]; break;
    case 11:
      oldSP = sp;
      sp--;
      return stack[oldSP&255];
      break;
    }
    return 0;
  }

  void writeToMemory(int index, int value) {
    switch (index) {
    case 0: break;
    case 1: registers[0] = value; break;
    case 2: registers[1] = value; break;
    case 3: registers[2] = value; break;
    case 4: registers[3] = value; break;
    case 5: registers[4] = value; break;
    case 6: registers[5] = value; break;
    case 7: registers[6] = value; break;
    case 8: registers[7] = value; break;
    case 9: registers[8] = value; break;
    case 10:
      cout<<"ADDR: "<<addr<<endl;
      ram[addr] = value;
      break;
    case 11:
      sp++;
      stack[sp&255] = value;
      break;
    }
    return;
  }

  void setAddr() {
    addr = registers[6] + registers[7]*256 + registers[8]*65536;
    cout<<addr<<endl;
    return;
  }

  void incSP() {
    sp++;
    return;
  }

  void decSP() {
    sp--;
    return;
  }

  void branch() {
    pc = addr;
    return;
  }

  void step() {
    pc++;
    return;
  }

  void loadProgram(ifstream& file) {
    string line;
    char l1;
    char l2;
    file.get(l1);
    file.get(l2);
    int len = hexToDec(l1, l2);
   
    for(int i = 0; i<len*4; i++) {
      char c1;
      char c2;
      file.get(c1);
      file.get(c2);
      ram[i] = hexToDec(c1, c2);
    }
  }
  void fetch() {
    inst = ram[pc*4];
    in1 = ram[pc*4 + 1];
    in2 = ram[pc*4 + 2];
    out = ram[pc*4 + 3];
    return;
  }
};


void emulate(memory &mem) {
  cout<<"Instruction: "<<mem.pc<<endl;
  mem.fetch();    // Fetch the next instruction from ram
  mem.setAddr();
  cout<<mem.addr<<endl;
  // local instruction variables
  int inst = mem.inst;
  int in1 = mem.in1;
  int in2 = mem.in2;
  int out = mem.out;
  cout<<inst<<" "<<in1<<" "<<in2<<" "<<out<<endl;
  // Resolving inputs to their numeric values
  if (!(inst & 128)) in1 = mem.readFromMemory(in1);
  if (!(inst & 64)) in2 = mem.readFromMemory(in2);

  // Getting the instruction without the immidiate flags
  inst = inst & 63;

  switch (inst) {
  case 0:    // ADD
    mem.writeToMemory(out, (in1 + in2)&255);
    if (in1+in2 > 255) mem.cf = true;
    else mem.cf = false;
    if (in1+in2 == 0) mem.zf = true;
    else mem.zf = false;
    cout<<in1<<" ADD "<<in2<<" = "<<((in1+in2)&255)<<" cf: "<<mem.cf<<" zf: "<<mem.zf<<endl;
    break;
  case 1:    // SUB
    mem.writeToMemory(out, in1 + (256-in2)&255);
    if (in1-in2 >= 0) mem.cf = true;
    else mem.cf = false;
    if (in1+in2 == 0) mem.zf = true;
    else mem.zf = false;
    cout<<in1<<" SUB "<<in2<<" = "<<((in1+(256-in2))&255)<<" cf: "<<mem.cf<<" zf: "<<mem.zf<<endl;
    break;
  case 2:    // OR
    mem.writeToMemory(out, (in1 | in2)&255);
    mem.cf = false;
    if (in1|in2 == 0) mem.zf = true;
    else mem.zf = false;
    cout<<in1<<" OR "<<in2<<" = "<<((in1 | in2)&255)<<" cf: "<<mem.cf<<" zf: "<<mem.zf<<endl;
    break;
  case 3:    // NOR
    mem.writeToMemory(out, 256-(in1 | in2)&255);
    mem.cf = true;
    if (256-(in1 | in2) == 0) mem.zf = true;
    else mem.zf = false;
    cout<<in1<<" NOR "<<in2<<" = "<<(256-(in1 | in2)&255)<<" cf: "<<mem.cf<<" zf: "<<mem.zf<<endl;
    break;
  case 4:    // AND
    mem.writeToMemory(out, (in1 & in2)&255);
    mem.cf = true;
    if (in1 & in2 == 0) mem.zf = true;
    else mem.zf = false;
    cout<<in1<<" AND "<<in2<<" = "<<((in1 & in2)&255)<<" cf: "<<mem.cf<<" zf: "<<mem.zf<<endl;
    break;
  case 5:    // NAND
    mem.writeToMemory(out, 256-(in1 & in2));
    mem.cf = false;
    if (256-(in1&in2) == 0) mem.zf = true;
    else mem.zf = false;
    cout<<in1<<" NAND "<<in2<<" = "<<(256-(in1 & in2))<<" cf: "<<mem.cf<<" zf: "<<mem.zf<<endl;
    break;
  case 6:    // XOR
    mem.writeToMemory(out, (in1^in2)&255);
    mem.cf = true;
    if (in1^in2 == 0) mem.zf = true;
    else mem.zf = false;
    cout<<in1<<" XOR "<<in2<<" = "<<(in1^in2)<<" cf: "<<mem.cf<<" zf: "<<mem.zf<<endl;
    break;
  case 7:    // XNOR
    mem.writeToMemory(out, (256-(in1^in2))&255);
    mem.cf = true;
    if (256-(in1^in2) == 0) mem.zf = true;
    else mem.zf = false;
    cout<<in1<<" XNOR "<<in2<<" = "<<(256-(in1^in2)&255)<<" cf: "<<mem.cf<<" zf: "<<mem.zf<<endl;
    break;
  case 8:    // SHR
    mem.writeToMemory(out, (int(floor((in1 + in2)/2)))&255);
    mem.cf = false;
    if (floor((in1 + in2)/2) == 0) mem.zf = true;
    else mem.zf = false;
    cout<<in1<<" SHR "<<in2<<" = "<<((int(floor((in1 + in2)/2)))&255)<<" cf: "<<mem.cf<<" zf: "<<mem.zf<<endl;
    break;
  case 9:    // BIZ
    if (mem.zf) {
      mem.branch();
    }
    break;
  case 10:    // BNZ
    if (!mem.zf) {
      mem.branch();
    }
    break;
  case 11:    // BIC
    if (mem.cf) {
      mem.branch();
    }
    break;
  case 12:    // BNC
    if (!mem.cf) {
      mem.branch();
    }
    break;
  case 63:
    mem.halt = true;
    cout<<"HALT"<<endl;
    break;
  }
  mem.step();
  return;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    cout<<"ERROR: please specify a file!"<<endl;
    return 1;
  }

  ifstream fi(argv[1]);
  if (!fi.is_open()) {
    cout<<"ERROR: couldn't open file: "<<argv[1]<<endl;
  }
  memory mem;
  mem.loadProgram(fi);
  while(!mem.halt) emulate(mem);

  fi.close();
  return 0;
}

