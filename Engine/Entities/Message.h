class Message
{
public:
	Message(int message_type, int message_subtype);
	virtual ~Message();

	int GetMessageType() const { return m_MessageType; }
	int GetMessageSubtype() const { return m_MessageSubtype; }

protected:
	int m_MessageType;
	int m_MessageSubtype;
};
