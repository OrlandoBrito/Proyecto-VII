Imports System.IO
Imports System.IO.Ports
Imports System.Threading
Public Class PcArduinoUsbPin13OnOff
    Shared _continue As Boolean
    Shared _serialPort As SerialPort

    Private Sub PcArduinoUsbPin13OnOff_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ComboBox1.Items.Add("com0")
        ComboBox1.Items.Add("com1")
        ComboBox1.Items.Add("com2")
        ComboBox1.Items.Add("com3")
        ComboBox1.Items.Add("com4")
        ComboBox1.Items.Add("com5")
        ComboBox1.Items.Add("com6")
        ComboBox1.Items.Add("com7")
        ComboBox1.Items.Add("com8")
        ComboBox1.Items.Add("com9")
        ComboBox1.Items.Add("com10")
        ComboBox1.Items.Add("com11")
        ComboBox1.Items.Add("com12")
        ComboBox1.Items.Add("com13")
        ComboBox1.Items.Add("com14")
        ComboBox1.Items.Add("com15")


    End Sub

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        SerialPort1.Open()
        SerialPort1.Write("1")
        SerialPort1.Close()


    End Sub

    Private Sub Button2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button2.Click
        SerialPort1.Open()
        SerialPort1.Write("0")
        SerialPort1.Close()
    End Sub

    Private Sub ComboBox1_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ComboBox1.SelectedIndexChanged
        
    End Sub

    Private Function Val() As String
        Throw New NotImplementedException
    End Function

    Private Sub Button3_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button3.Click

        SerialPort1.PortName = ComboBox1.Text 'change com port to match your Arduino port
        SerialPort1.Close()
        SerialPort1.BaudRate = 9600
        SerialPort1.DataBits = 8
        SerialPort1.Parity = Parity.None
        SerialPort1.StopBits = StopBits.One
        SerialPort1.Handshake = Handshake.None
        SerialPort1.Encoding = System.Text.Encoding.Default 'very important!
    End Sub

    Private Sub Button4_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button4.Click
        If SerialPort1.IsOpen = True Then
            SerialPort1.Close()
        End If
    End Sub
End Class
