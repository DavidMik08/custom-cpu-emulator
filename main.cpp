#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <cstring>
using namespace std;

enum instructions {
  ADD,
  SUB,
  OR,
  NOR,
  AND,
  NAND,
  XOR,
  XNOR,
  SHR,

  BNC,
  BIC,
  BNZ,
  BIZ,

  BRK = 63
};


int hexToDec(char hex) { // a function to turn one hex digit into a intager
  int dec;
  if (isdigit(hex)) return (hex - '0'); // if the hex digit is 0-9 just return it as a int
  // else return the matching a-f to its decimal value
  switch (hex) {
  case 'a':
  case 'A':
    return 10;
  case 'b':
  case 'B':
    return 11;
  case 'c':
  case 'C':
    return 12;
  case 'd':
  case 'D':
    return 13;
  case 'e':
  case 'E':
    return 14;
  case 'f':
  case 'F':
    return 15;
  }
  return -1; // If an invalid characer is given the function will return -1 to signal that something went wrong
}

// Function to load the program into ram
void loadProgram(ifstream& inFile, vector<int>& ram) {
  int len = 0;
  char c;
  for (int i = 5; i >= 0; i--) {  // Calculating the lenght based on the first 6 hex digits
    c = inFile.get();
    len += hexToDec(c) * pow (16, i);
  }

  // Storing the instructions from the input file to ram
  int code = 0;
  for (int i = 0; i < len; i++) {
    c = inFile.get();
    code = hexToDec(c) * 16; // Decoding the second digit to decimal
    c = inFile.get();
    code += hexToDec(c);     // Decoding the first digit to decimal
    ram[i] = code;                      // Stores the code in ram at the next address
    //cout << "DEBUG: Loaded: " << ram[i] << " at address: " << i << endl;  // Debug to see whats getting stored at what address
  }
  return;
}

void fetch(int& inst, int& in1, int& in2, int& out, int pc, vector<int> ram) {  // fetch the next instruction
  inst = ram[pc*4 + 0];
  in1  = ram[pc*4 + 1];
  in2  = ram[pc*4 + 2];
  out  = ram[pc*4 + 3];
  return;
}


bool executeInstruction(int inst, int in1, int in2, int* out, bool& cf, bool& zf, bool& halt) {  // executes a single instruction
  switch (inst & 63) {
  case ADD:
    
    *out = (in1 + in2) & 255;

    if ((in1 + in2) > 255) cf = true;
    else cf = false;

    if (((in1 + in2) & 255) == 0) zf = true;
    else zf = false;
    
    break;
    
    
  case SUB:

    *out = (in1 + (256-in2)) & 255;  // Add the 8-bit twos complement of in2 to in1

    if ((in1 + (256-in2)) > 255) cf = true;
    else cf = false;

    if (((in1 + (256-in2)) & 255) == 0) zf = true;
    else zf = false;
    
    break;


  case OR:
    
    *out = (in1 | in2) & 255;

    cf = false;

    if (((in1 | in2) & 255) == 0) zf = true;
    else zf = false;
    
    break;
    
    
  case NOR:

    *out = 255 - ((in1 | in2) & 255); 

    cf = false;

    if ((255 - ((in1 | in2) & 255)) == 0) zf = true;
    else zf = false;
    
    break;


  case AND:
    
    *out = (in1 & in2) & 255;

    cf = false;

    if (((in1 & in2) & 255) == 0) zf = true;
    else zf = false;
    
    break;
    
    
  case NAND:

    *out = 255 - ((in1 & in2) & 255);

    cf = false;

    if ((255 - ((in1 & in2) & 255)) == 0) zf = true;
    else zf = false;
    
    break;


  case XOR:
    
    *out = (in1 ^ in2) & 255;

    cf = true;

    if (((in1 ^ in2) & 255) == 0) zf = true;
    else zf = false;
    
    break;
    
    
  case XNOR:

    *out = 255 - ((in1 ^ in2) & 255); 

    cf = true;

    if ((255 - ((in1 ^ in2) & 255)) == 0) zf = true;
    else zf = false;
    
    break;
  

  case SHR:

    *out = 255 - ((in1 | in2) & 255); 

    if ((in1 + in2) > 255) cf = true;
    else cf = false;

    if (((in1 + in2) & 254) == 0) zf = true;
    else zf = false;
    
    break;




  case BNC:

    if (!cf)  return true;
    
    break;


  case BIC:
    
    if (cf)  return true;
    
    break;
    
    
  case BNZ:

    if (!zf) return false;
    
    break;


  case BIZ:

    if (zf)  return true;
    
    break;



  case BRK:

    halt = true;
    
    break;
  }

  return false;
}

string decToHex(int value) {
  string hex;
  int d1;
  int d2;
  // Calculating first hex digit
  if ((value % 16) < 10) d1 = (value % 16) + '0';
  else switch (value % 16) {
    case 10: d1 = 'A'; break;
    case 11: d1 = 'B'; break;
    case 12: d1 = 'C'; break;
    case 13: d1 = 'D'; break;
    case 14: d1 = 'E'; break;
    case 15: d1 = 'F'; break;
  }

  // Calculating second hex digit
  if ((value / 16) < 10) d2 = (value / 16) + '0';
  else switch (value / 16) {
    case 10: d2 = 'A'; break;
    case 11: d2 = 'B'; break;
    case 12: d2 = 'C'; break;
    case 13: d2 = 'D'; break;
    case 14: d2 = 'E'; break;
    case 15: d2 = 'F'; break;
  }
  hex+=d2;
  hex+=d1;
  return hex;
}

void readPorts(vector<int>& inPorts) {
  ifstream inPortsFile("inPorts.txt");
  int value = 0;
  char c;
  for (int i = 0; i < 4; i++) { 
    c = inPortsFile.get();
    value = hexToDec(c) * 16; // Decoding the second digit to decimal
    c = inPortsFile.get();
    value += hexToDec(c);     // Decoding the first digit to decimal
    inPorts[i] = value;       // Storing the value to the input ports
  }
  inPortsFile.close();
  return;
}

void writeToPorts(vector<int>& outPorts) {
  ofstream outPortsFile("outPorts.txt");
  for (int i = 0; i < 4; i++) outPortsFile << decToHex(outPorts[i]);
  outPortsFile.close();
  return;
}

void emulateInstruction(vector<int>& registers, vector<int>& inPorts, vector<int>& outPorts, vector<int>& ram, vector<int>& stack, int& pc, int& sp, bool& halt, bool& cf, bool& zf) {
  registers[0] = 0;  // Set the zero register to zero
  int addr = registers[7] + registers[8] * pow(2, 8) + registers[9] * pow(2, 16);  // Set the main address based on the address registers
  int inst, in1, in2, out;  // Variables that are going to store the current instruction
  bool branch = false;  // Turns on if the CPU is branching to another part of code

  readPorts(inPorts); // Reads form a file that modules can write to
  
  fetch(inst, in1, in2, out, pc, ram);


  // Check if the first input isn't an immidiate number
  if ((inst & 64) == 0) {
    if (in1 < 10)       in1 = registers[in1];
    else if (in1 < 14)  in1 = inPorts[in1-10];
    else if (in1 == 14) in1 = ram[addr];
    else if (in1 == 15) in1 = stack[sp--];
  }

  // Check if the second input isn't an immidiate number
  if ((inst & 128) == 0) {
    if (in2 < 10)       in2 = registers[in2];
    else if (in2 < 14)  in2 = inPorts[in2-10];
    else if (in2 == 14) in2 = ram[addr];
    else if (in2 == 15) in2 = stack[sp--];
  }


  // Setting an output pointer to point at the specified output
  int* outPtr;
  if (out < 10)       outPtr = &registers[out];
  else if (out < 14)  outPtr = &outPorts[out-10];
  else if (out == 14) outPtr = &ram[addr];
  else if (out == 15) outPtr = &stack[++sp];


  // execute the current instruction and return if the CPU should branch or not
  branch = executeInstruction(inst, in1, in2, outPtr, cf, zf, halt);

  writeToPorts(outPorts);  // Writes the ports to a shared file that the modules can read from
  
  if (branch) {
    pc = addr;
    return;
  }
  
  pc++;
  return;
}

int main(int argc, char** argv) {
  bool dump = false;
  int dumpSize = 0;
  if (argc < 2) {
    cout << "ERROR: Expected an input file!"       << endl
	 << "Corect usage:"                        << endl
	 << "  " << argv[0] << " <file1.hex>"      << endl
	 << endl
         << "Please provide an file to interpret." << endl;
    return 1;
  } else if (argc > 3) {
    cout << "DUMP" << endl;
    cout << argv[2] << endl;
    if (strcmp(argv[2], "-d") == 0) {
      cout << "DUMP 2" << endl;
      dumpSize = stoi(argv[3]);
      dump = true;
    }
  }
  vector<int> registers(10, 0), inPorts(4, 0), outPorts(4, 0), ram(pow(2, 24), 0), stack(pow(2, 8), 0);  // All of the CPUs memory
  int pc = 0, sp = 0;  // The program counter and stack pointer
  bool halt = false;   // If halt is set to true the program will stop
  bool cf = false, zf = false;  // ALU flags

  ifstream program(argv[1]);
  if (!program.is_open()) { // Check if file opened successfully
    cout << "ERROR: Can't open file \"" << argv[1] << "\"" << endl;
    return 1;
  }
  
  loadProgram(program, ram);

  while (!halt) emulateInstruction(registers, inPorts, outPorts, ram, stack, pc, sp, halt, cf, zf);  // while the CPU hasn't halted emulate the next instruction
  if (dump) {
    for (int i = 0; i < dumpSize; i++) {
      cout << decToHex(i) << ": " << ram[i] << endl;
    }
  }
  program.close();
  
  return 0;
}
