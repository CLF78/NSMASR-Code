class RandTileGenerator
	Types = [:none, :horz, :vert, :both]
	Specials = [nil, :vdouble_top, :vdouble_bottom]

	def initialize
		@sections = {}
	end

	def section(*namelist)
		raise "what" unless namelist.all?{|x| x.is_a?(String)}
		@current_section = (@sections[namelist] ||= {entries: []})
		yield
	end

	def random(range, type=:both, numbers=nil)
		case range
		when Range
			# Regular handling
			numbers = range if numbers.nil?
			@current_section[:entries] << {range: range, type: type, numbers: numbers.to_a}
		when Numeric
			# One number
			random(range..range, type, numbers)
		when Enumerable
			# An array or something else similar
			numbers = range if numbers.nil?
			range.each { |r| random(r, type, numbers) }
		end
	end

	def pack
		# first, work out an offset for every section and entry
		# also, collect the data for each individual entry into an Array
		current_offset = 8 + (@sections.count * 4)
		all_entry_data = []

		@sections.each_pair do |name, section|
			section[:offset] = current_offset
			current_offset += 8

			section[:entries].each do |entry|
				entry[:offset] = current_offset
				all_entry_data << entry[:numbers]
				current_offset += 8
			end
		end

		# assign an offset to each section name list
		namelist_offsets = {}
		@sections.each_key do |namelist|
			namelist_offsets[namelist] = current_offset
			current_offset += 4 + (4 * namelist.size)
		end

		# assign an offset to each piece of entry data
		data_offsets = {}
		all_entry_data.uniq!

		all_entry_data.each do |data|
			data_offsets[data] = current_offset
			current_offset += data.size
		end

		# assign an offset to each section name
		name_offsets = {}
		@sections.each_key do |namelist|
			namelist.each do |name|
				name_offsets[name] = current_offset
				current_offset += name.size + 1
			end
		end

		# now pack it all together
		header = ['NwRT', @sections.count].pack('a4 N')
		offsets = @sections.each_value.map{|s| s[:offset]}.pack('N*')

		section_data = @sections.each_pair.map do |namelist, section|
			namelist_offset = namelist_offsets[namelist] - section[:offset]
			entry_count = section[:entries].count

			entry_data = section[:entries].map do |entry|
				lower_bound = entry[:range].min
				upper_bound = entry[:range].max

				count = entry[:numbers].count

				type_sym, special_sym = entry[:type].to_s.split('_', 2).map(&:to_sym)
				type_id = Types.index(type_sym)
				special_id = Specials.index(special_sym)
				type = type_id | (special_id << 2)

				num_offset = data_offsets[entry[:numbers]] - entry[:offset]

				[lower_bound, upper_bound, count, type, num_offset].pack('CCCC N')
			end

			[namelist_offset, entry_count].pack('NN') + entry_data.join
		end

		namelist_data = @sections.each_key.map do |namelist|
			puts "Writing list: #{namelist.inspect}"
			count = namelist.size
			c_offsets = namelist.map{|n| name_offsets[n] - namelist_offsets[namelist]}
			puts "Offsets: #{c_offsets.inspect}"

			[count].pack('N') + c_offsets.pack('N*')
		end

		output = [header, offsets]
		output += section_data
		output += namelist_data
		output += all_entry_data.map{|data| data.pack('C*')}
		output << @sections.keys.flatten.join("\0")
		output << "\0"
		output.join
	end


	def regular_terrain
		# Left Side
		random([0x10, 0x20, 0x30, 0x40], :vert)
		# Right Side
		random([0x11, 0x21, 0x31, 0x41], :vert)
		# Top Side
		random(2..7, :horz)
		# Bottom Side
		random(0x22..0x27, :horz)
		# Middle
		random(0x12..0x17)
	end

	def sub_terrain
		# Left Side
		random([0x18, 0x28, 0x38, 0x48], :vert)
		# Right Side
		random([0x19, 0x29, 0x39, 0x49], :vert)
		# Top Side
		random(0xA..0xF, :horz)
		# Bottom Side
		random(0x2A..0x2F, :horz)
		# Middle
		random(0x1A..0x1F)
	end
end


g = RandTileGenerator.new

regular_ts1 = %w(chika chika_u suichu_u suichu_u_cold nohara2)
regular_ts1 += %w()
regular_ts2 = %w(og_nohara)
regular_ts3 = %w()
newer = %w()
newer += %w()

regular_ts1.map!{ |x| "Pa1_#{x}" }
regular_ts2.map!{ |x| "Pa2_#{x}" }
regular_ts3.map!{ |x| "Pa3_#{x}" }
g.section(*regular_ts1, *regular_ts2, *regular_ts3, *newer) do
	g.regular_terrain
end

g.section(*%w(Pa1_magicsnow, Pa1_magicsnow_night)) do
	g.random(0xB4..0xB9, :horz)
end

g.section(*%w(Pa1_nohara)) do
		g.regular_terrain
		g.random([0x44, 0x54, 0x64, 0x74], :vert)
		g.random([0x45, 0x55, 0x65, 0x75], :vert)
		g.random(0x36..0x3B, :horz)
		g.random(0x46..0x4B)
		g.random(0x56..0x5B, :horz)
		g.random([0x96, 0xA6, 0xB6, 0xC6], :vert)
		g.random([0x97, 0xA7, 0xB7, 0xC7], :vert)
		g.random(0x78..0x7D, :horz)
		g.random(0x88..0x8D)
		g.random(0x98..0x9D, :horz)
end

g.section(*%w(Pa1_fortress_lava)) do
		g.random(0x40..0x47)
		g.random(0x50..0x55, :horz)
		g.random([0x29, 0x39, 0x49, 0x59], :vert)
		g.random([0x2A, 0x3A, 0x4A, 0x5A], :vert)
		g.random([0x48, 0x58], :vert)
		g.random([0x1B, 0x1C, 0x1D, 0x2B, 0x2C, 0x2D, 0x3B, 0x3C, 0x3D, 0x4B])
		g.random(0x4C..0x4D, :horz)
end

g.section(*%w(Pa2_trees, Pa2_trees_night)) do
		g.random([0x01, 0x07])
		g.random([0x10, 0x16])
		g.random([0x12, 0x17])
		g.random([0x21, 0x27])
		g.random([0x40, 0x36])
		g.random([0x42, 0x37])
end

File.open('C:\Dolphin\NSMASR\NewerRes\RandTiles.bin', 'wb') do |f|
	f.write g.pack
end

