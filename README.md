# custom-cpu-emulator
Updated version with the Von Neumann architecture.

# overview
This is an emulator for a custom 8-bit CPU.
It has s 24-bit address bus that is used for both branching and ram access.
The instructions are made of 4 bytes each written in hexadecimal, one for the instruction opcode, two for the inputs and one for the output (where we want to store the result).

Example:

00010203

this would add register 1 and register 2 and save the result into register 3

00 - add

01 - r1

02 - r2

03 - r3

# instruction set
The instruction set includes the following istructions:

OPCODE (HEX)  ASSEMBLY  DESCRIPTION

ALU instructions:

00            add       adds the inputs

01            sub       subtracts the inputs

02            or        logical or

03            nor       logical nor

04            and       logical and

05            nand      logical nand

06            xor       logical xor

07            xnor      logical xnor

08            shr       adds the inputs and shifts the bits of the result by one place to the right


Branch instructions:

09            biz       branches to the address on the address bus if the zero flag is on

0A            bnz       branches to the address on the address bus if the zero flag is off

0B            bic       branches to the address on the address bus if the carry flag is on

0C            bnc       branches to the address on the address bus if the carry flag is off


Other instructions:

3F            brk       halts the CPU


# flag handeling
The zero flag is turned on when executing an ALU instruction if its result is zero,
its turned off when executing an ALU instruction if the result isn't zero.
The carry flag is turned on when executing the following instructions:
nor, and, xor, xnor,
it can also turn on if the result of adding is grater than 255,
or if the result is positive when subtracting.

# immidiate values
If we want to use immidiate values (hex numbers 0-FF) instead of registers and other memory, we add 80 (hex) to the instruction if the first input is an immidiate value and we add 40 (hex) if the second input is an immidiate value.

# start of the code
Firstly the first two digits of our hex code must be the lenght of the code interpreted as the number of instructions that we want to store to ram, if we make this number smaller than the actual number of instructions the CPU will only load the amount of instructions specified by this number ant the rest of the instructions will be ignored,
this limits the number of instructions to 255 for now, 
but i plan to make it use the first six digits so we can have 4194304 instructions (they are going to be limmited by ram size)

# memory
The CPU also has RAM and stack memory.
It uses the Von Neumann arhitecture so the instructions are saved in ram and fetched when needed.

# RAM
To access RAM we first need to set the address that we want to read from or write to in r7, r8 and r9, 
r7 holds the lowest byte, r8 holds the middle byte and r9 holds the highest byte.

To set the address to 00A0FF(HEX) we would write something like this:

C0FF0007C0A00008C0000009

C0 - add with both immidiate inputs

FF - first input (immidiate value FF)

00 - second input (immidiate value 00)

07 - store to register 7 (Low byte of the address)



C0 - add with both immidiate inputs

A0 - first input (immidiate value A0)

00 - second input (immidiate value 00)

08 - store to register 8 (Middle byte of the address)


C0 - add with both immidiate inputs

00 - first input (immidiate value 00)

00 - second input (immidiate value 00)

09 - store to register 9 (High byte of the address)


Then if we want to write to that address we use ram as the output of an instruction, lets say that for this example we want to store the result of adding 5 (hex) to register 1:

40010510

40 - add with the second immidiate input

01 - first input (register 1)

05 - second input (immidiate value 05)

10 - store to ram


So if we wanted some code that stores the result of adding 5 (hex) to register 1 in ram at the address 00A0FF that would look like this:

C0FF0007C0A00008C000000940010510


But if we want to read from ram we can just set the address the same way as for storing and then use ram as one of the inputs instead of the output, so if we wanted to subtract 3 (hex) from ram at the current address and store the result to register 1 the code would look like this:

41100301

41 - subtract with the second immidiate input

10 - first input (ram)

03 - second input (immidiate value 03)

01 - store to register 1


# stack memory
The stack works in a simmilar way, but we dont need to set the address.
If we want to push to the stack, lets say we were pushing the result of adding 10 (hex) to register 5, we would do something like this:

40051011

40 - add with the second immidiate input

05 - first input (register 5)

10 - second input (immidiate value 10)

11 - store to the stack


If we want to pop from the stack, lets say that we want to pop a value and add 20 (hex) to it and store the result to register 4, we would write something like this:

40112004

40 - add with the second immidiate input

11 - first input (the stack)

20 - second input (immidiate value 20)

04 - store to register 4



# example program
So a program that adds 5 and 3 into r1 and subtracts 2 from it and stores the result to ram at the address 000DF3 would look like this:

06C0050301C0F30007C00D0008C0000009410102103F000000

06 - code lenght

C0050301 - add 05 and 03 into r1

C0F30007 - add F3 and 00 into r7 (setting the low byte of the address)

C00D0008 - add 0D and 00 into r8 (setting the middle byte of the address)

C0000009 - add 00 and 00 into r9 (setting the high byte of the address)

41010210 - subtract 02 from r1 and store the result in ram

3F000000 - halt the CPU

