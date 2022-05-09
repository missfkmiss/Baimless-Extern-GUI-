#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include "globals.hh"

DWORD GetProcId(const wchar_t* procName)
{
	DWORD procId = 0;
	//cria uma handle e definimos ela igual a todos os processos do windows
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//checamos se ela é invalida
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		//definimos o processo e o tamanho dele
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);
		//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

		//definimos procEntry como todos os processos
		if (Process32First(hSnap, &procEntry))
		{
			//criamos um loop entre os processos
			do
			{
				//se encontrarmos o processo pelo nome que definimos no argumento
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					//define procId = pid do processo procurado;
					procId = procEntry.th32ProcessID;
					//fecha o loop
					break;
				}
			} while (Process32Next(hSnap, &procEntry));

		}
	}
	//fecha handle
	CloseHandle(hSnap);
	// retorna o process id
	return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;

	//cria uma handle pro processo pedindo o Modulo (x64 = TH32CS_SNAPMODULE | x86 = TH32CS_SNAPMODULE32)
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	//checa se a handle pro nosso processo é valida
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		//se for criamos um modulo onde iremos setar o valor dele mais em baixo;
		MODULEENTRY32 modEntry;
		//calculamos o tamanho do modulo que ele poderá armazenar
		modEntry.dwSize = sizeof(modEntry);
		//definimos nosso modEntry = hSnap (modulo do processo)
		if (Module32First(hSnap, &modEntry))
		{
			//criamos um loop que vai rodar até ele encontrar o module name que queremos
			do
			{
				//pegamos todos os modulos do processo e comparamos com nosso process id
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					//se encontrar setamos modBaseAddr = modulo que queremos;
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					//finalizamos o loop
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	//fechamos a handle
	CloseHandle(hSnap);
	//retornamos o valor do modulo
	return modBaseAddr;
}