library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned.all;
use IEEE.std_logic_arith.all;
use WORK.useful_functions_pkg.all;

entity regfile is
  generic (
    NWP           : integer := 2;
    NRP           : integer := 3;
    AW            : integer := 10;
    DW            : integer := 16
  );
  port (
    clock         : in  std_logic;
    reset         : in  std_logic;
    enable        : in  std_logic;
    we_v          : in  std_logic_vector(NWP-1 downto 0);
    re_v          : in  std_logic_vector(NRP-1 downto 0);
    waddr_v       : in  std_logic_vector(NWP*AW-1 downto 0);
    raddr_v       : in  std_logic_vector(NRP*AW-1 downto 0);
    input_data_v  : in  std_logic_vector(NWP*DW-1 downto 0);
    ram_output_v  : out std_logic_vector(NRP*DW-1 downto 0)
  );
end regfile;

architecture rtl of regfile is
  component regfile_core
    generic (
      AW            : integer :=  5;
      DW            : integer := 32
    );
    port (
      clock         : in  std_logic;
      reset         : in  std_logic;
      enable        : in  std_logic;
      we            : in  std_logic;
      re            : in  std_logic;
      waddr         : in  std_logic_vector(AW-1 downto 0);
      raddr         : in  std_logic_vector(AW-1 downto 0);
      input_data    : in  std_logic_vector(DW-1 downto 0);
      ram_output    : out std_logic_vector(DW-1 downto 0)
    );
  end component;
  constant NREGS : integer := 2**AW;
  type banksel_type is array (NRP-1 downto 0) of std_logic_vector(log2c(NWP)-1 downto 0);
  signal banksel_v    : std_logic_vector(NRP*log2c(NWP)-1 downto 0);
  signal ia_sel       : banksel_type;
  signal ram_output_i : std_logic_vector((NRP*NWP*DW)-1 downto 0);
begin
  nwp_nrp_bram_instance_0 : entity WORK.regfile_core(READ_ASYNC)
    generic map (
      AW            => AW-log2c(NWP),
      DW            => DW
    )
    port map (
      clock         => clock,
      reset         => reset,
      enable        => enable,
      we            => we_v(0),
      re            => re_v(0),
      waddr         => waddr_v(AW*(0+1)-log2c(NWP)-1 downto AW*0),
      raddr         => raddr_v(AW*(0+1)-log2c(NWP)-1 downto AW*0),
      input_data    => input_data_v(DW*(0+1)-1 downto DW*0),
      ram_output    => ram_output_i(DW*((0*NRP+0)+1)-1 downto DW*(0*NRP+0))
    );

  nwp_nrp_bram_instance_3 : entity WORK.regfile_core(READ_ASYNC)
    generic map (
      AW            => AW-log2c(NWP),
      DW            => DW
    )
    port map (
      clock         => clock,
      reset         => reset,
      enable        => enable,
      we            => we_v(1),
      re            => re_v(0),
      waddr         => waddr_v(AW*(1+1)-log2c(NWP)-1 downto AW*1),
      raddr         => raddr_v(AW*(0+1)-log2c(NWP)-1 downto AW*0),
      input_data    => input_data_v(DW*(1+1)-1 downto DW*1),
      ram_output    => ram_output_i(DW*((1*NRP+0)+1)-1 downto DW*(1*NRP+0))
    );

  nwp_nrp_bram_instance_1 : entity WORK.regfile_core(READ_ASYNC)
    generic map (
      AW            => AW-log2c(NWP),
      DW            => DW
    )
    port map (
      clock         => clock,
      reset         => reset,
      enable        => enable,
      we            => we_v(0),
      re            => re_v(1),
      waddr         => waddr_v(AW*(0+1)-log2c(NWP)-1 downto AW*0),
      raddr         => raddr_v(AW*(1+1)-log2c(NWP)-1 downto AW*1),
      input_data    => input_data_v(DW*(0+1)-1 downto DW*0),
      ram_output    => ram_output_i(DW*((0*NRP+1)+1)-1 downto DW*(0*NRP+1))
    );

  nwp_nrp_bram_instance_4 : entity WORK.regfile_core(READ_ASYNC)
    generic map (
      AW            => AW-log2c(NWP),
      DW            => DW
    )
    port map (
      clock         => clock,
      reset         => reset,
      enable        => enable,
      we            => we_v(1),
      re            => re_v(1),
      waddr         => waddr_v(AW*(1+1)-log2c(NWP)-1 downto AW*1),
      raddr         => raddr_v(AW*(1+1)-log2c(NWP)-1 downto AW*1),
      input_data    => input_data_v(DW*(1+1)-1 downto DW*1),
      ram_output    => ram_output_i(DW*((1*NRP+1)+1)-1 downto DW*(1*NRP+1))
    );

  nwp_nrp_bram_instance_2 : entity WORK.regfile_core(READ_ASYNC)
    generic map (
      AW            => AW-log2c(NWP),
      DW            => DW
    )
    port map (
      clock         => clock,
      reset         => reset,
      enable        => enable,
      we            => we_v(0),
      re            => re_v(2),
      waddr         => waddr_v(AW*(0+1)-log2c(NWP)-1 downto AW*0),
      raddr         => raddr_v(AW*(2+1)-log2c(NWP)-1 downto AW*2),
      input_data    => input_data_v(DW*(0+1)-1 downto DW*0),
      ram_output    => ram_output_i(DW*((0*NRP+2)+1)-1 downto DW*(0*NRP+2))
    );

  nwp_nrp_bram_instance_5 : entity WORK.regfile_core(READ_ASYNC)
    generic map (
      AW            => AW-log2c(NWP),
      DW            => DW
    )
    port map (
      clock         => clock,
      reset         => reset,
      enable        => enable,
      we            => we_v(1),
      re            => re_v(2),
      waddr         => waddr_v(AW*(1+1)-log2c(NWP)-1 downto AW*1),
      raddr         => raddr_v(AW*(2+1)-log2c(NWP)-1 downto AW*2),
      input_data    => input_data_v(DW*(1+1)-1 downto DW*1),
      ram_output    => ram_output_i(DW*((1*NRP+2)+1)-1 downto DW*(1*NRP+2))
    );

  banksel_v(log2c(NWP)*(0+1)-1 downto log2c(NWP)*0) <= raddr_v(AW*(0+1)-1 downto AW*(0+1)-log2c(NWP));
  banksel_v(log2c(NWP)*(1+1)-1 downto log2c(NWP)*1) <= raddr_v(AW*(1+1)-1 downto AW*(1+1)-log2c(NWP));
  banksel_v(log2c(NWP)*(2+1)-1 downto log2c(NWP)*2) <= raddr_v(AW*(2+1)-1 downto AW*(2+1)-log2c(NWP));

  process (ram_output_i, banksel_v)
    variable ia_sel_part : integer range 0 to NWP-1;
  begin
    ia_sel(0) <= banksel_v(log2c(NWP)*(0+1)-1 downto log2c(NWP)*0);
    ia_sel_part := conv_integer(ia_sel(0));
    case ia_sel_part is
      when 0      => ram_output_v(DW*(0+1)-1 downto DW*0) <= ram_output_i(DW*(0+0*NRP+1)-1 downto DW*(0+0*NRP));
      when 1      => ram_output_v(DW*(0+1)-1 downto DW*0) <= ram_output_i(DW*(0+1*NRP+1)-1 downto DW*(0+1*NRP));
      when others => ram_output_v(DW*(0+1)-1 downto DW*0) <= (others => '0');
    end case;
  end process;

  process (ram_output_i, banksel_v)
    variable ia_sel_part : integer range 0 to NWP-1;
  begin
    ia_sel(1) <= banksel_v(log2c(NWP)*(1+1)-1 downto log2c(NWP)*1);
    ia_sel_part := conv_integer(ia_sel(1));
    case ia_sel_part is
      when 0      => ram_output_v(DW*(1+1)-1 downto DW*1) <= ram_output_i(DW*(1+0*NRP+1)-1 downto DW*(1+0*NRP));
      when 1      => ram_output_v(DW*(1+1)-1 downto DW*1) <= ram_output_i(DW*(1+1*NRP+1)-1 downto DW*(1+1*NRP));
      when others => ram_output_v(DW*(1+1)-1 downto DW*1) <= (others => '0');
    end case;
  end process;

  process (ram_output_i, banksel_v)
    variable ia_sel_part : integer range 0 to NWP-1;
  begin
    ia_sel(2) <= banksel_v(log2c(NWP)*(2+1)-1 downto log2c(NWP)*2);
    ia_sel_part := conv_integer(ia_sel(2));
    case ia_sel_part is
      when 0      => ram_output_v(DW*(2+1)-1 downto DW*2) <= ram_output_i(DW*(2+0*NRP+1)-1 downto DW*(2+0*NRP));
      when 1      => ram_output_v(DW*(2+1)-1 downto DW*2) <= ram_output_i(DW*(2+1*NRP+1)-1 downto DW*(2+1*NRP));
      when others => ram_output_v(DW*(2+1)-1 downto DW*2) <= (others => '0');
    end case;
  end process;

end rtl;
