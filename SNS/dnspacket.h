#ifndef DNSPACKET_H
#define DNSPACKET_H

// standart headers
#include <cstring>
#include <vector>
#include <string>

// system headers
#include <WinSock2.h>

namespace SNS
{
	//////////////////////////////////////////////////////////////////////////

	// http://www.maxblogs.ru/articles/dns-domain-name-system
	enum MessageType{ QUERY = 0, ANSWER = 1 };
	enum OperationCode{ STANDART = 0, INVERSION = 1, STATUS = 2, NOTIFY = 4, UPDATE = 5 };
	enum ReturnCode{ NOERR = 0, FORMATERR = 1, SERVERR = 2, NAMEERR = 3, NAMENOTEXIST = 4, NOTIMPL = 5 };

	enum RequestType{ A = 1, NS = 2, CNAME = 5, SOA = 6, MB = 7, WKS = 11, 
						PTR = 12, HINFO = 13, MINFO = 14, MX = 15, TXTISDN = 16, 
						AXFR = 252, ANY = 255 };

	//////////////////////////////////////////////////////////////////////////

	class DnsHeader
	{
	protected:
		#pragma pack(push, 1)
		struct DnsStruct
		{
			// �������������
			unsigned short ident;
			// �����
			unsigned short flags;
			// ���������� ��������
			unsigned short qcount;
			// ���������� �������
			unsigned short acount;
			// ���������� ���� �������
			unsigned short lcount;
			// ���������� �������������� �������
			unsigned short ocount;
		} header;
		#pragma pack(pop)
	public:
		DnsHeader(const char* rawPacket);
		virtual size_t size() const;
		// ����� � ������� ������������ �����
		// ���������� ��������� �� ��������� ���� �� ���������
		virtual char* dump(char* out) const;
		virtual ~DnsHeader() = default;

		// ������������� �������
		unsigned short getHeaderId() const;

		// ���������� ��������
		unsigned short getQueryCount() const;
		void setQueryCount(unsigned short c);
		// ���������� �������
		unsigned short getAnswerCount() const;
		void setAnswerCount(unsigned short c);
		// ���������� ������������
		unsigned short getAccessCount() const;
		void setAccessCount(unsigned short c);
		// ���������� ���������������
		unsigned short getAdditionCount() const;
		void setAdditionCount(unsigned short c);

		// ���� ������� ��� ������
		MessageType getMessageType() const;
		void setMessageType(MessageType t);

		// ��� �������
		OperationCode getOperationCode() const;
		void setOperationCode(OperationCode t);

		// ������������ �����
		bool isAuthorityAnswer() const;
		void setAuthorityAnswer(bool t);

		// ����� ��������� 512 ����
		bool isTruncated() const;
		void setTruncated(bool t);

		// ��������� ��������
		bool needRecursion() const;
		void setNeedRecursion(bool t);

		// �������� ��������
		bool allowRecursion() const;
		void setAllowRecursion(bool t);

		// ��� ������
		ReturnCode getReturnCode() const;
		void setReturnCode(ReturnCode t);
	};

	//////////////////////////////////////////////////////////////////////////

	class DnsRequest:public DnsHeader
	{
	protected:
		#pragma pack(push,1)
		struct ReqFlag
		{
			// ���
			unsigned short qtype;
			// �����
			unsigned short qclass;
		};
		#pragma pack(pop)
	private:
		std::vector<std::pair<std::string, ReqFlag>> items;
	public:
		DnsRequest(const char* rawPacket);
		virtual size_t size() const override;
		virtual char* dump(char* out) const override;
		virtual ~DnsRequest() = default;

		size_t reqCount() const;
		std::string getAddress(size_t i) const;
		RequestType getType(size_t i) const;
		unsigned short getClass(size_t i) const;
		const std::pair<std::string, ReqFlag>& getRaw(size_t i) const;
	};

	//////////////////////////////////////////////////////////////////////////

	class DnsResponse:public DnsRequest
	{
		#pragma pack(push,1)
		struct RespFlag
		{
			// ����� �����
			unsigned long TTL;
			// ����� ������
			unsigned short len;
		};
		#pragma pack(pop)
		std::vector<
			std::pair<
				std::pair<std::string, ReqFlag>,
				std::pair<RespFlag, std::vector<char>>
			>
		> answer;
		std::vector<
			std::pair<
				std::pair<std::string, ReqFlag>,
				std::pair<RespFlag, std::vector<char>>
			>
		> legacy;
		std::vector<
			std::pair<
				std::pair<std::string, ReqFlag>,
				std::pair<RespFlag, std::vector<char>>
			>
		> addition;
		size_t sectionSize(const std::vector<std::pair<std::pair<std::string, ReqFlag>, std::pair<RespFlag, std::vector<char>>>>& section) const;
		char* appSection(const std::vector<std::pair<std::pair<std::string, ReqFlag>, std::pair<RespFlag, std::vector<char>>>>& section, char* out) const;
	public:
		DnsResponse(const char* rawPacket);
		virtual size_t size() const override;
		virtual char* dump(char* out) const override;
		virtual ~DnsResponse() = default;

		void addRawAnswer(const std::pair<std::string, ReqFlag>& query, unsigned long TTL, const std::vector<char>& raw);
		void addRawLegacy(const std::pair<std::string, ReqFlag>& query, unsigned long TTL, const std::vector<char>& raw);
		void addRawAddition(const std::pair<std::string, ReqFlag>& query, unsigned long TTL, const std::vector<char>& raw);
	};

	//////////////////////////////////////////////////////////////////////////
}

#endif // !DNSPACKET_H
