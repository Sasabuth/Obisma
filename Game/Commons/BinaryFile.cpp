#include "pch.h"
#include "BinaryFile.h"

#include <fstream>
#include <assert.h>


std::unique_ptr<BinaryFile> BinaryFile::LoadFile(const wchar_t * fileName)
{
	std::ifstream ifs;

	//	ファイルオープン
	ifs.open(fileName, std::ios::in | std::ios::binary);

	//	読み込み失敗時、強制終了
	assert(ifs);

	//	実際にファイル内容を読み取る
	auto bin = std::make_unique<BinaryFile>();

	//	ファイルサイズを取得
	ifs.seekg(0, std::fstream::end);
	std::streamoff eofPos = ifs.tellg();
	ifs.clear();
	ifs.seekg(0, std::fstream::beg);
	std::streamoff begPos = ifs.tellg();
	bin->m_size = (unsigned int)(eofPos - begPos);

	//	読み込むためのメモリを確保
	bin->m_data.reset(new char[bin->m_size]);

	//	ファイル先頭からバッファへコピー 
	ifs.read(bin->m_data.get(), bin->m_size);

	//	ファイルクローズ
	ifs.close();

	return bin;
}

BinaryFile::BinaryFile()
{
	m_size = 0;
}

BinaryFile::~BinaryFile()
{
	m_size = 0;
}

BinaryFile::BinaryFile(BinaryFile* in)
{
	m_data = std::move(in->m_data);
	m_size = in->m_size;
}
