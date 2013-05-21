package micropolisj.gui;

import java.awt.*;
import javax.swing.*;

public class TranslationTool extends JFrame
{
	public TranslationTool()
	{
		JButton btn = new JButton("Hello World!");
		add(btn, BorderLayout.CENTER);

		pack();
		setLocationRelativeTo(null);
		setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
	}
}
