#include <stdio.h>

int binarySearch(int* arr, int n, int targetIdx){
	int l = 0, r = n-1;
	while(l <= r){
		int mid = l + (r - l) / 2;
		if(targetIdx == arr[mid]){
			return mid;
		}
		else if(arr[mid] < targetIdx){
			l = mid + 1;
		}
		else
			r = mid - 1;
	}
	return -1;
}

int main(){
	int arr[] = {2,3,5,6,7,9,13,23};
	printf("Target Index: %d", binarySearch(arr, sizeof(arr)/sizeof(arr[0]), 7));
	return 0;
}
