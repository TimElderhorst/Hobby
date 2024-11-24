// reading a text file
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <string>
#include <sstream>
#include <bitset>
#include <cstdint>  // For uint8_t
using namespace std;

// Function to check if a string represents a number
bool isNumber(const string& str) {
    bool hasDecimalPoint = false;

    for (size_t i = 0; i < str.length(); i++) {
        if (isdigit(str[i])) {
            continue; // Skip digits
        } else if (str[i] == '.' && !hasDecimalPoint) {
            hasDecimalPoint = true; // Allow one decimal point
        } else if (i == 0 && (str[i] == '+' || str[i] == '-')) {
            continue; // Skip sign character at the beginning
        } else {
            return false; // Non-numeric character found
        }
    }
    return !str.empty();
}

int main () {
  string line;
  string dest;
  string comp;
  string jump;
  uint8_t destout = 0;
  uint8_t compout = 0;
  uint8_t jumpout = 0;
  uint16_t aout = 0;
  uint16_t out = 0;
  bool cinstr = false;
  bool empty = false;
  int address = 0;
  int n = 16;
  int value = 0;
  ifstream inputfile ("C:/Users/timel/OneDrive/Documenten/Hobby/Nand to Tetris/Pong.asm");
  ofstream outputfile ("C:/Users/timel/OneDrive/Documenten/Hobby/Nand to Tetris/Output.hack");
  
  // Initialization: define symbol table
  std::unordered_map<std::string, int> symbolTable;

  // Initialization: add entries to the symbol table
  symbolTable["R0"] = 0;
  symbolTable["R1"] = 1;
  symbolTable["R2"] = 2;
  symbolTable["R3"] = 3;
  symbolTable["R4"] = 4;
  symbolTable["R5"] = 5;
  symbolTable["R6"] = 6;
  symbolTable["R7"] = 7;
  symbolTable["R8"] = 8;
  symbolTable["R9"] = 9;
  symbolTable["R10"] = 10;
  symbolTable["R11"] = 11;
  symbolTable["R12"] = 12;
  symbolTable["R13"] = 13;
  symbolTable["R14"] = 14;
  symbolTable["R15"] = 15;
  symbolTable["SCREEN"] = 16384;
  symbolTable["KBD"] = 24576;
  symbolTable["SP"] = 0;
  symbolTable["LCL"] = 1;
  symbolTable["ARG"] = 2;
  symbolTable["THIS"] = 3;
  symbolTable["THAT"] = 4;


  if (inputfile.is_open())
  {
    // First pass: scanning for labels (xxx)
    while ( getline (inputfile,line)){
      std::size_t position = line.find("//");
      if (position != std::string::npos) {
        line = line.substr(0, position);
      }
      // find '(', and ')'
      if (line[0] == '(') {
        position = line.find(")");
        if (position != std::string::npos) {
          line = line.substr(1, position-1);
          // Add a new entry to the symbol table using 'line' as the symbol name
          symbolTable[line] = address;
          address--;
        }        
      }
      

      line.erase(0, line.find_first_not_of(" \t")); // Trim leading whitespace
      line.erase(line.find_last_not_of(" \t") + 1); // Trim trailing whitespace
      if (!line.empty()) {
        address++;
      }
    }

    // Second pass
    inputfile.clear(); // Clear EOF and other flags
    inputfile.seekg(0); // Reset file cursor to the beginning
    while ( getline (inputfile,line) )
    {
      // Reset all variables
      dest = "";
      comp = "";
      jump = "";
      destout = 0;
      compout = 0;
      jumpout = 0;
      cinstr = false;
      empty = false;
      aout = 0;
      out = 0;
      value = 0;

      // Find the position of the substring "//"
      std::size_t position = line.find("//");

      // Check if "//" was found, insert a null-terminator at the location of "//"
      if (position != std::string::npos) {
        line = line.substr(0, position);
      }
      // Trim whitespace
      line.erase(0, line.find_first_not_of(" \t")); // Trim leading whitespace
      line.erase(line.find_last_not_of(" \t") + 1); // Trim trailing whitespace
      
      // Check if line is empty. If empty, do not print line to outputfile
      if (line.empty()) {
        empty = true;
        continue;
      }

      // If 1st character is "(" the line is a label. Do not print to outputfile.
      if (line[0] == '(') {
        continue;
      }
      
      //If line starts with '@' this is A-instruction. Remove '@' and convert from decimal to binary
      if (line[0] == '@') {
        line = line.substr(1);
        //Check if input contains text. If so, this is a symbol. If necessary, add this symbol to the symbol table.
        if (!isNumber(line)) {
          // Check if the input exists in the symbol table
          if (symbolTable.find(line) != symbolTable.end()) {
            value = symbolTable[line];
            line = std::to_string(value);
          } else {
          // Add to symbol table
          symbolTable[line] = n;
          value = symbolTable[line];
          line = std::to_string(value);         
          n++;
          }
        }       
     }
      
      //Else this is C-instruction. Split into dest, comp and jump bits. Convert to correct binary values
      else {
        cinstr = true;
        position = line.find(";");
        if (position != std::string::npos) {
          comp = line.substr(0,position);
          jump = line.substr(position + 1);
        }
        else {
          comp = line;
        }
        position = line.find("=");
        if (position != std::string::npos) {
          dest = line.substr(0,position);
          comp = line.substr(position+1);
        }
        line = "";
      }
      
      //C-instruction: out = 111 + jump + comp + dest
          // Convert dest into 3 bit value
          if(dest.find("A")!= std::string::npos){
            destout |= (1 << 2);
          }
          else {destout |= (0 << 2);}
          if(dest.find("D")!= std::string::npos){
            destout |= (1 << 1);
          }
          else {destout |= (0 << 1);}
          if(dest.find("M")!= std::string::npos){
            destout |= (1 << 0);
          }
          else {destout |= (0 << 0);}

          // Convert comp into 7 bit value
          if ((comp.find("D|A") != std::string::npos)|(comp.find("D|M") != std::string::npos)){
            compout |= (0 << 5); 
            compout |= (1 << 4);
            compout |= (0 << 3);
            compout |= (1 << 2);
            compout |= (0 << 1);
            compout |= (1 << 0);
          }
          else if ((comp.find("D&A") != std::string::npos)|(comp.find("D&M") != std::string::npos)){
            compout |= (0 << 5); 
            compout |= (0 << 4);
            compout |= (0 << 3);
            compout |= (0 << 2);
            compout |= (0 << 1);
            compout |= (0 << 0);
          }
          else if ((comp.find("A-D") != std::string::npos)|(comp.find("M-D") != std::string::npos)){
            compout |= (0 << 5); 
            compout |= (0 << 4);
            compout |= (0 << 3);
            compout |= (1 << 2);
            compout |= (1 << 1);
            compout |= (1 << 0);
          }
          else if ((comp.find("D-A") != std::string::npos)|(comp.find("D-M") != std::string::npos)){
            compout |= (0 << 5); 
            compout |= (1 << 4);
            compout |= (0 << 3);
            compout |= (0 << 2);
            compout |= (1 << 1);
            compout |= (1 << 0);
          }
          else if ((comp.find("D+A") != std::string::npos)|(comp.find("D+M") != std::string::npos)){
            compout |= (0 << 5); 
            compout |= (0 << 4);
            compout |= (0 << 3);
            compout |= (0 << 2);
            compout |= (1 << 1);
            compout |= (0 << 0);
          }
          else if ((comp.find("A-1") != std::string::npos)|(comp.find("M-1") != std::string::npos)){
            compout |= (1 << 5); 
            compout |= (1 << 4);
            compout |= (0 << 3);
            compout |= (0 << 2);
            compout |= (1 << 1);
            compout |= (0 << 0);
          }
          else if (comp.find("D-1") != std::string::npos){
            compout |= (0 << 5); 
            compout |= (0 << 4);
            compout |= (1 << 3);
            compout |= (1 << 2);
            compout |= (1 << 1);
            compout |= (0 << 0);
          }
          else if ((comp.find("A+1") != std::string::npos)|(comp.find("M+1") != std::string::npos)){
            compout |= (1 << 5); 
            compout |= (1 << 4);
            compout |= (0 << 3);
            compout |= (1 << 2);
            compout |= (1 << 1);
            compout |= (1 << 0);
          }
          else if (comp.find("D+1") != std::string::npos){
            compout |= (0 << 5); 
            compout |= (1 << 4);
            compout |= (1 << 3);
            compout |= (1 << 2);
            compout |= (1 << 1);
            compout |= (1 << 0);
          }
          else if ((comp.find("-A") != std::string::npos)|(comp.find("-M") != std::string::npos)){
            compout |= (1 << 5); 
            compout |= (1 << 4);
            compout |= (0 << 3);
            compout |= (0 << 2);
            compout |= (1 << 1);
            compout |= (1 << 0);
          }
          else if (comp.find("-D") != std::string::npos){
            compout |= (0 << 5); 
            compout |= (0 << 4);
            compout |= (1 << 3);
            compout |= (1 << 2);
            compout |= (1 << 1);
            compout |= (1 << 0);
          }
          else if ((comp.find("!A") != std::string::npos)|(comp.find("!M") != std::string::npos)){
            compout |= (1 << 5); 
            compout |= (1 << 4);
            compout |= (0 << 3);
            compout |= (0 << 2);
            compout |= (0 << 1);
            compout |= (1 << 0);
          }
          else if (comp.find("!D") != std::string::npos){
            compout |= (0 << 5); 
            compout |= (0 << 4);
            compout |= (1 << 3);
            compout |= (1 << 2);
            compout |= (0 << 1);
            compout |= (1 << 0);
          }
          else if ((comp.find("A") != std::string::npos)|(comp.find("M") != std::string::npos)){
            // Binary value = 0110000;
            compout |= (1 << 5); 
            compout |= (1 << 4);
            compout |= (0 << 3);
            compout |= (0 << 2);
            compout |= (0 << 1);
            compout |= (0 << 0);
          }
          else if (comp.find("D") != std::string::npos){
            // Binary value = 0001100
            compout |= (0 << 5); 
            compout |= (0 << 4);
            compout |= (1 << 3);
            compout |= (1 << 2);
            compout |= (0 << 1);
            compout |= (0 << 0);
          }
          else if (comp.find("-1") != std::string::npos){
            compout |= (1 << 5); 
            compout |= (1 << 4);
            compout |= (1 << 3);
            compout |= (0 << 2);
            compout |= (1 << 1);
            compout |= (0 << 0);
          }
          else if (comp.find("1") != std::string::npos){
            compout |= (1 << 5); 
            compout |= (1 << 4);
            compout |= (1 << 3);
            compout |= (1 << 2);
            compout |= (1 << 1);
            compout |= (1 << 0);
          }
          else {
            compout |= (1 << 5); 
            compout |= (0 << 4);
            compout |= (1 << 3);
            compout |= (0 << 2);
            compout |= (1 << 1);
            compout |= (0 << 0);
          }
          
          // Convert comp MSB to 1, if comp contains M
          if (comp.find("M") != std::string::npos){
            compout |= (1 << 6);
          }
          else {compout |= (0 << 6);}

          // Convert jump into 3 bit value
          if (jump.find("JMP") != std::string::npos){
            jumpout |= (1 << 2);
            jumpout |= (1 << 1);
            jumpout |= (1 << 0);
          }
          else if (jump.find("JLE") != std::string::npos){
            jumpout |= (1 << 2);
            jumpout |= (1 << 1);
            jumpout |= (0 << 0);
          }
          else if (jump.find("JNE") != std::string::npos){
            jumpout |= (1 << 2);
            jumpout |= (0 << 1);
            jumpout |= (1 << 0);
          }
          else if (jump.find("JLT") != std::string::npos){
            jumpout |= (1 << 2);
            jumpout |= (0 << 1);
            jumpout |= (0 << 0);
          }
          else if (jump.find("JGE") != std::string::npos){
            jumpout |= (0 << 2);
            jumpout |= (1 << 1);
            jumpout |= (1 << 0);
          }
          else if (jump.find("JEQ") != std::string::npos){
            jumpout |= (0 << 2);
            jumpout |= (1 << 1);
            jumpout |= (0 << 0);
          }
          else if (jump.find("JGT") != std::string::npos){
            jumpout |= (0 << 2);
            jumpout |= (0 << 1);
            jumpout |= (1 << 0);
          }
          else {
            jumpout |= (0 << 2);
            jumpout |= (0 << 1);
            jumpout |= (0 << 0);
          }
        //C-instruction: out = 111 + jump + comp + dest
        if (cinstr == true){
          out = 0b111 << 13;
          out |= (compout & 0b1111111) << 6;
          out |= (destout & 0b111) << 3;
          out |= (jumpout & 0b111);
        }

        //A-instruction: out = 0 + line
            // Convert line into 15 bit value 
        else {
          std::istringstream(line) >> out;
          std::bitset<16> binary(out);
          out |= (0 << 15);
        } 

      // Testing
//      std::cout << "C-instr: " << cinstr << endl;
//      std::cout << "Comp: " << comp << endl;
//      std::cout << "Dest: " << dest << endl;
//      std::cout << "Jump: " << jump << endl;
//      std::cout << "Comp out: " << std::bitset<7>(compout) << endl;
//      std::cout << "Dest out: " << std::bitset<3>(destout) << endl;
//      std::cout << "Jump out: " << std::bitset<3>(jumpout) << endl;
//      std::cout << "Out: " << std::bitset<16>(out) << endl << endl;

      // Output to file
      outputfile << std::bitset<16>(out) << '\n';
    }
    
    // Print the entire symbol table
    std::cout << "Symbol Table:\n";
    std::cout << "-----------------\n";
    std::cout << "Symbol\tAddress\n";
    std::cout << "-----------------\n";

    for (const auto& entry : symbolTable) {
        std::cout << entry.first << "\t" << entry.second << "\n";
    }
    
    // Close input and output file
    inputfile.close();
    outputfile.close();
  }

  else cout << "Unable to open file"; 
  
  return 0;
}