/*
 *  Multi2Sim
 *  Copyright (C) 2015  Shi Dong (dong.sh@husky.neu.edu)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "gtest/gtest.h"

#include <lib/cpp/IniFile.h>
#include <lib/cpp/Error.h>
#include <memory/System.h>
#include <memory/Manager.h>
#include <arch/x86/disassembler/Disassembler.h>
#include <arch/x86/emulator/Emulator.h>
#include <arch/x86/timing/Timing.h>
#include <arch/x86/timing/Cpu.h>
#include <arch/x86/timing/Thread.h>

namespace x86 
{

TEST(TestX86TimingFetchStage, simple_fetch)
{


	// CPU configuration file
	std::string config_string =
			"[ General ]\n"
			"[ TraceCache ]\n"
			"Present = f";
	misc::IniFile config_ini;
	config_ini.LoadFromString(config_string);
	try
	{
		Timing::ParseConfiguration(&config_ini);
	}
	catch (misc::Exception &e)
	{
		std::cerr << "Exception in reading x86 configuration" <<
				e.getMessage() << "\n";
		ASSERT_TRUE(false);
	}

	// Get instance of Timing
	Emulator *emulator = Emulator::getInstance();
	Timing *timing = Timing::getInstance();

	// Memory configuration file
	std::string mem_config_string =
			"[ General ]\n"
			"[ Module mod-mm ]\n"
			"Type = MainMemory\n"
			"Latency = 50\n"
			"BlockSize = 64\n"
			"[ Entry core-1 ]\n"
			"Arch = x86\n"
			"Core = 0\n"
			"Thread = 0\n"
			"Module = mod-mm\n";
	misc::IniFile mem_config_ini;
	mem_config_ini.LoadFromString(mem_config_string);

	// Configure
	try
	{
		mem::System::getInstance()->ReadConfiguration(&mem_config_ini);
	}
	catch (misc::Exception &e)
	{
		std::cerr << "Exception in reading mem configuration" <<
				e.getMessage() << "\n";
		ASSERT_TRUE(false);
	}


	// Code to execute
	// mov eax, 1;
	// int 0x80;
	unsigned char code[128] = {0};
	code[0] = 0xB8;
	code[1] = 0x01;
	code[2] = 0x00;
	code[3] = 0x00;
	code[4] = 0x00;
	code[5] = 0xcd;
	code[6] = 0x80;

	// Create a context]
	Context *context = emulator->newContext();
	context->Initialize();
	auto mmu = misc::new_shared<mem::Mmu>("mmu");
	context->mmu = mmu;
	mem::Mmu::Space space("space", mmu.get());
	context->mmu_space = &space;
	mem::Memory *memory = context->getMemory();
	memory->setHeapBreak(misc::RoundUp(memory->getHeapBreak(),
				mem::Memory::PageSize));

	// Allocate memory and save the instructions into memory
	mem::Manager manager(memory);
	unsigned eip = manager.Allocate(sizeof(code), 128);
	memory->Write(eip, sizeof(code), (const char *)code);

	// Update context status, including eip
	context->setUinstActive(true);
	context->setState(Context::StateRunning);
	context->getRegs().setEip(eip);

	// Map the thread onto cpu hardware
	Cpu *cpu = Timing::getInstance()->getCpu();
	Thread *thread = cpu->getThread(0, 0);
	thread->MapContext(context);
	thread->Schedule();

	// Fetch
	thread->setFetchNeip(eip);
	timing->Run();

	// After fetching,
	esim::Engine *engine = esim::Engine::getInstance();
	for (int i = 0; i < 500; i++)
	{
		EXPECT_EQ(2, thread->getFetchQueueSize());
		EXPECT_EQ(7, thread->getFetchQueueOccupency());

		// Process events
		engine->ProcessEvents();
		timing->Run();
	}
	engine->ProcessEvents();
	timing->Run();
	EXPECT_EQ(0, thread->getFetchQueueSize());
	EXPECT_EQ(0, thread->getFetchQueueOccupency());
}

}  // namespace x86