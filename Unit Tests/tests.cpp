#include "../Connection Library/host.h"
#include "WinUnit\Include\WinUnit.h"

BEGIN_TEST(Connecting) {
	Host host;
	
	WIN_ASSERT_FALSE(host.Connect("", 100));
	WIN_ASSERT_FALSE(host.Connect("thisisnotvalidaddress", 100));
	WIN_ASSERT_FALSE(host.Connect("localhost", 10));

	host.Disconnect();
} END_TEST

BEGIN_TEST(Disconnecting) {
	Host host;
	WIN_ASSERT_FALSE(host.Disconnect());
} END_TEST

BEGIN_TEST(SendingData) {
	WIN_ASSERT_FALSE(3 > 4);
} END_TEST