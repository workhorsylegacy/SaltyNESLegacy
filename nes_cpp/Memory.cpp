

#include "Globals.h"


	 Memory::Memory(NES* nes, int byteCount){
		this->nes = nes;
		mem = new short[byteCount];
		memLength = byteCount;
	}
	
	 void Memory::stateLoad(ByteBuffer* buf){
		
		if(mem==NULL)mem=new short[memLength];
		buf.readByteArray(mem);
		
	}
	
	 void Memory::stateSave(ByteBuffer* buf){
		
		buf.putByteArray(mem);
		
	}
	
	 void Memory::reset(){
		for(int i=0;i<mem.length;i++)mem[i] = 0;
	}
	
	 int Memory::getMemSize(){
		return memLength;
	}
	
	 void Memory::write(int address, short value){
		mem[address] = value;
	}
	
	 short Memory::load(int address){
		return mem[address];
	}
	
	 void Memory::dump(string file){
		dump(file,0,mem.length);
	}
	
	 void Memory::dump(string file, int offset, int length){
		
		char[] ch = new char[length];
		for(int i=0;i<length;i++){
			ch[i] = (char)mem[offset+i];
		}
		
		try{
			
			File f = new File(file);
			FileWriter writer = new FileWriter(f);
			writer.write(ch);
			writer.close();
			//System.out.println("Memory dumped to file "+file+".");
			
		}catch(IOException ioe){
			//System.out.println("Memory dump to file: IO Error!");
		}
		
		
	}
	
	 void Memory::write(int address, short* array, int length){
	
		if(address+length > mem.length)return;
		System.arraycopy(array,0,mem,address,length);
		
	}
	
	 void Memory::write(int address, short* array, int arrayoffset, int length){
		
		if(address+length > mem.length)return;
		System.arraycopy(array,arrayoffset,mem,address,length);
		
	}
	
	 void Memory::destroy(){
	
		nes = NULL;
		mem = NULL;
		
	}
	
};
