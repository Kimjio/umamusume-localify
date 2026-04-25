#pragma once

namespace System::Collections::Generic
{
	template<typename K, typename V>
	class Dictionary
	{
	public:
		struct Entry
		{
		public:
			int hashCode;
			int next;
			K key;
			V value;
		};
	};
}
