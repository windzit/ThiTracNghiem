#include "../include/Score.h"
#include "../include/StorageManager.h"

Score::Score(const Score& other)
{
	root = nullptr;

	dsDiemThi* p = other.root;
	while (p)
	{
		insert(p->diemthi);
		p = p->next;
	}
}

Score& Score::operator=(const Score& other)
{
	if (this == &other)
		return *this;

	clear();
	root = nullptr;

	dsDiemThi* p = other.root;
	while (p)
	{
		insert(p->diemthi);
		p = p->next;
	}

	return *this;
}

void Score::clear() {
	while (root) {
		dsDiemThi* tmp = root;
		root = root->next;
		delete tmp;
	}
}

bool Score::insert(const DiemThi& DIEMTHI) {
	dsDiemThi* node = new dsDiemThi (DIEMTHI);

	if (!root) {
		root = node;
		return true;
	}

	dsDiemThi* cur = root;
	dsDiemThi* par = nullptr;
	int cmp;
	while (cur)
	{
		cmp = strcmp(DIEMTHI.MAMH, cur->diemthi.MAMH);
		if (cmp == 0) {
			delete node;
			return false;
		}
		if (cmp < 0) break;

		par = cur;
		cur = cur->next;
	}
	
	// Fisrt index
	if (!par) {
		node->next = root;
		root = node;
	}
	else { // Mid & Last index
		node->next = cur;
		par->next = node;
	}

	return true;	
}

bool Score::remove(const char* MAMH) {
	if (!root) 
		return false;

	dsDiemThi* cur = root;
	dsDiemThi* par = nullptr;
	int cmp = -1;	
	while (cur) {
		cmp = strcmp(cur->diemthi.MAMH, MAMH);
		if (cmp == 0)
			break;

		par = cur;
		cur = cur->next;
	}

	// Not Found
	if (cmp != 0)
		return false;

	// Fisrt Index
	if (!par) {
		root = cur->next;
		delete cur;
		return true;
	}

	par->next = cur->next;
	delete cur;
	return true;
}

dsDiemThi* Score::find(const char* MAMH) {
	if (!root || !MAMH)
		return nullptr;

	dsDiemThi* cur = root;
	while (cur) {
		int cmp = strcmp(cur->diemthi.MAMH, MAMH);
		if (cmp == 0)
			return cur;
		if (cmp > 0)
			return nullptr;

		cur = cur->next;
	}

	return nullptr;
}

const dsDiemThi* Score::find(const char* MAMH) const {
	if (!root || !MAMH)
		return nullptr;

	const dsDiemThi* cur = root;
	while (cur) {
		int cmp = strcmp(cur->diemthi.MAMH, MAMH);
		if (cmp == 0)
			return cur;
		if (cmp > 0)
			return nullptr;

		cur = cur->next;
	}

	return nullptr;
}

int Score::count() const {
    int cnt = 0;
    dsDiemThi* cur = root;
    while (cur) {
        cnt++;
        cur = cur->next;
    }
    return cnt;
}

bool Score::empty() const {
    return root == nullptr;
}

bool Score::save(const std::string& MASV) {
    return true;
}

bool Score::load(const std::string& MASV) {
    return true;
}