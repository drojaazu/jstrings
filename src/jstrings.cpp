
#include "jstrings.hpp"
#include "enc_validator.hpp"
#include <cstring>
#include <string>
#include <vector>

namespace motoi
{

using namespace std;

vector<found_string> find(istream & is, encoding_validator const & enc, size_t const minimum_match_length)
{

	vector<found_string> out;

	// work buffer - input file is read/procesed in chunks of buffer_size bytes
	uptr<byte_t> buffer(new byte_t[buffer_size]);
	byte_t * p_buffer = buffer.get();

	/*
		Pointer to the end of the data within the buffer
		This will be (buffer + buffer_size) most of the time except for the
		final read of the stream where there will likely be less than a full
		buffer worth of data
	*/
	byte_t * p_buffer_end {nullptr};

	// the current byte(s) we are examining within the buffer
	byte_t * p_curr {nullptr};

	/*
		Due to the variable length nature of bytes, we set a "boundary"
		near the end of the buffer, at:
		(p_buffer_end - enc_validator.max_sequence_length)
		This prevents false negatives when a multibyte character is split
		across buffer reads.
		Once the read pointer is past the boundary, the remaining,
		unprocessed bytes are moved to the front of the buffer
		and the buffer is filled with the next read from the stream
		less the bytes moved to the front.
	*/
	byte_t * p_boundary {nullptr};

	// Track our location within the source data
	size_t source_offset {0};

	/*
		The address of the start of a valid string in terms of
		the *source data* rather than the buffer
	*/
	size_t valid_start_offset {0};

	/*
		Start and end pointers for a string of valid data with
		the buffer
	*/
	byte_t * p_valid_start {nullptr};
	byte_t * p_valid_end {nullptr};

	/*
		If p_valid_start is set and we have not yet determined the
		end of the string, but we have reached the boundary and
		are reloading the buffer, the valid data found so far needs
		to be perserved in a secondary buffer. In this case, we
		go ahead and allocate a new found_string with the data so
		far and set the flag below, which will indicate that the
		remaining data should be appended to the last found_string
		in the list.
	*/
	bool crossed_boundary {false};

	size_t valid_string_length {0};
	size_t advance_read_pointer_count {0};
	int remaining_count {0};

	while (is.good())
	{
		// prepare buffer
		if (p_valid_start != nullptr)
		{
			// we have a valid string in progress
			out.push_back({valid_start_offset, {p_valid_start, p_curr}});
			crossed_boundary = true;
		}

		// always clear the buffer since we don't know how many bytes
		// will actually be read
		memset((char *) (p_buffer + remaining_count), 0, (buffer_size - remaining_count));
		is.read((char *) (p_buffer + remaining_count), (buffer_size - remaining_count));
		streamsize buffer_read_size = is.gcount() + remaining_count;

		p_buffer_end = p_buffer + buffer_read_size;
		p_curr = p_buffer;

		/*
			if we're on the final buffer read (is.eof()) or if the data we read is smaller
			than the max sequence length of the encoding, then don't bother setting a boundary
		*/
		if ((buffer_read_size < enc.max_seq_len()) || is.eof())
			p_boundary = p_buffer + buffer_read_size;
		else
			p_boundary = p_buffer + (buffer_read_size - enc.max_seq_len());

		while (p_curr < p_boundary)
		{
			auto valid_byte_count {enc(p_curr)};
			if (valid_byte_count > 0)
			{
				// found a valid sequence
				if (p_valid_start == nullptr)
				{
					// starting a new valid string
					p_valid_start = p_curr;
					valid_start_offset = source_offset;
				}

				++valid_string_length;
				// move to the next byte after this valid grouping
				advance_read_pointer_count = valid_byte_count;
			}
			else
			{
				// hit an invalid byte
				if (p_valid_start != nullptr)
				{
					if (valid_string_length >= minimum_match_length)
					{
						// valid_start was set, so we're finishing a valid string
						p_valid_end = p_curr;
						if (crossed_boundary)
						{
							out.back().second.insert(out.back().second.end(), p_buffer, p_valid_end);
						}
						else
						{
							// clang-format off
							out.push_back ({
								valid_start_offset,
								{p_valid_start, p_valid_end}
							});
							// clang-format on
						}

						if (out.size() >= out.max_size())
							return out;
					}

					if (crossed_boundary)
						crossed_boundary = false;

					p_valid_start = nullptr;
					p_valid_end = nullptr;
				}

				valid_string_length = 0;

				// move to next byte as this one was invalid
				advance_read_pointer_count = 1;
			}

			// increase pointer and offset
			p_curr += advance_read_pointer_count;
			source_offset += advance_read_pointer_count;
		}

		remaining_count = p_buffer_end - p_curr;
		if (remaining_count > 0)
			copy(p_curr, p_buffer_end, p_buffer);
	}

	// finished processing all input
	// check if we have an unfinished valid string
	if ((p_valid_start != nullptr) && (valid_string_length >= minimum_match_length))
	{
		// clang-format off
		out.push_back ({
			valid_start_offset,
			{p_valid_start, p_curr}
		});
		// clang-format on
	}

	return out;
}
} // namespace motoi