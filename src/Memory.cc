/*
SaltyNES Copyright (c) 2012-2014 Matthew Brennan Jones <matthew.brennan.jones@gmail.com>
vNES Copyright (c) 2006-2011 Jamie Sanders

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "SaltyNES.h"


Memory::Memory(NES* nes, int byteCount) {
	this->nes = nes;
	mem = new vector<uint16_t>(byteCount);
	memLength = byteCount;
}

Memory::~Memory() {
	nes = nullptr;
	delete_n_null(mem);
}

void Memory::stateLoad(ByteBuffer* buf) {
	if(mem==nullptr)mem=new vector<uint16_t>(memLength);
	buf->readByteArray(mem);
}

void Memory::stateSave(ByteBuffer* buf) {
	buf->putByteArray(mem);	
}

void Memory::reset() {
	for(size_t i=0; i<mem->size(); i++)
		(*mem)[i] = 0;
}

int Memory::getMemSize() {
	return memLength;
}

void Memory::write(int address, uint16_t value) {
	(*mem)[address] = value;
}

uint16_t Memory::load(int address) {
	return (*mem)[address];
}

void Memory::dump(string file) {
	dump(file, 0, mem->size());
}

void Memory::dump(string file, int offset, int length) {
	char* ch = new char[length];
	for(int i=0; i<length; i++) {
		ch[i] = (char)(*mem)[offset+i];
	}
	
	try{
        ofstream writer(file.c_str(), ios::out|ios::binary);
		writer.write((char*)ch, length);
		writer.close();
		printf("Memory dumped to file \"%s\".\n", file.c_str());
		
	}catch(exception& ioe) {
		printf("%s\n", "Memory dump to file: IO Error!");
	}

	delete[] ch;
}

void Memory::write(int address, vector<uint16_t>* array, int length) {
	if(address+length > (int) mem->size())return;
	arraycopy_short(array, 0, mem, address, length);
}

void Memory::write(int address, vector<uint16_t>* array, int arrayoffset, int length) {
	if(address+length > (int) mem->size())return;
	arraycopy_short(array,arrayoffset,mem,address,length);
}


