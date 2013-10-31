library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_unsigned.all;
use IEEE.std_logic_arith.all;
use WORK.useful_functions_pkg.all;

entity regfile is
  generic (
    NWP           : integer := 1;
    NRP           : integer := 1;
    AW            : integer := 11;
    DW            : integer := 32
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
  signal ram_output_i : std_logic_vector((NRP*NWP*DW)-1 downto 0);
begin
  nwp_nrp_bram_instance_0 : entity WORK.regfile_core(READ_FIRST)
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

  ram_output_v(DW*(0+1)-1 downto DW*0) <= ram_output_i(DW*(0+1)-1 downto DW*0);
end rtl;
