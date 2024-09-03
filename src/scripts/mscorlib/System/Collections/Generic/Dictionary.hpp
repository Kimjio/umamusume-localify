#pragma once

namespace System
{
	namespace Collections
	{
		namespace Generic
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
	}
}
