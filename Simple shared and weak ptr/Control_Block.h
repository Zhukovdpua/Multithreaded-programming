#pragma once

struct Control_Block {
	int strongRef;
	int weakRef;
	Control_Block()
		:strongRef(0), weakRef(0) {}
};
